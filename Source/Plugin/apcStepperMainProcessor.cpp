#include <vector>
#include <regex>
#include "apcStepperMainProcessor.h"
#include "apcStepperMainEditor.h"

static const int apcPARAMETER_V1 = 1;




apcStepperMainProcessor::apcStepperMainProcessor()
        : juce::AudioProcessor(getBusesProperties()),
          parameters(*this, nullptr, "PARAMETERS", createParameterLayout()) {
    // Initialize parameters
//    numSteps = dynamic_cast<juce::AudioParameterInt *>(parameters.getParameter("steps"));
//    numInstruments = dynamic_cast<juce::AudioParameterInt *>(parameters.getParameter("instruments"));
    tempoParam = dynamic_cast<juce::AudioParameterInt *>(parameters.getParameter("tempo"));
    transposeParam = dynamic_cast<juce::AudioParameterInt *>(parameters.getParameter("transpose"));
    velocityScaleParam = dynamic_cast<juce::AudioParameterFloat *>(parameters.getParameter("velocityScale"));

    if (!tempoParam || !transposeParam || !velocityScaleParam) {
        juce::Logger::writeToLog("Error: Failed to initialize parameters!");
        return;
    }

    // Add parameter listeners
//    parameters.addParameterListener("steps", this);
//    parameters.addParameterListener("instruments", this);
    parameters.addParameterListener("tempo", this);
    parameters.addParameterListener("transpose", this);
    parameters.addParameterListener("velocityScale", this);


    // Set initial values
    *tempoParam = 98;
    *transposeParam = 0;
    *velocityScaleParam = 1.0f;
//
//    // TODO: Remove this hard-code and wire it up to a GUI:
//    *numSteps = 8;
//    *numInstruments = 8;

    // Initialize step track parameters
    for (int step = 0; step < 8; step++) {
        for (int trackNr = 0; trackNr < numSteps; trackNr++) {
            juce::String parameterID = "step_" + juce::String(step) + "_track_" + juce::String(trackNr);
            parameters.addParameterListener(parameterID, this);
        }
    }

    // Initialize FatButton  parameters
    for (int fatButtonNr = 0; fatButtonNr < numInstruments; fatButtonNr++) {
        juce::String parameterID = "fatButton_" + juce::String(fatButtonNr);
        parameters.addParameterListener(parameterID, this);
    }

    // Initialize slider parameters
    for (int sliderNr = 0; sliderNr < 8; sliderNr++) {
        juce::String parameterID = "slider_" + juce::String(sliderNr);
        parameters.addParameterListener(parameterID, this);
    }

    parameters.state.setProperty("parameterVersion", parameterVersion, nullptr);
//
//    // Initialize midiGrid
//    midiGrid.resize(numInstruments);
//    for (auto &row: midiGrid) {
//        row.resize(numSteps, false);
//    }

    // Initialize MIDI file and sequence
    midiFile.setTicksPerQuarterNote(96); // 1/8 note = 48 ticks

    // Create and add a track
    trackSequence = std::make_unique<juce::MidiMessageSequence>();
    midiFile.addTrack(*trackSequence);
}

apcStepperMainProcessor::~apcStepperMainProcessor() = default;

juce::AudioProcessor::BusesProperties apcStepperMainProcessor::getBusesProperties() {
#if JUCE_STANDALONE_APPLICATION
    return BusesProperties()
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true);
#else
    return BusesProperties()
        .withOutput("MIDI Out", juce::AudioChannelSet::disabled(), true);
#endif
}

bool apcStepperMainProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
#if JUCE_STANDALONE_APPLICATION
    return (layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo() &&
            layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo()) ||
           (layouts.getMainInputChannelSet() == juce::AudioChannelSet::disabled() &&
            layouts.getMainOutputChannelSet() == juce::AudioChannelSet::disabled());
#else
    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::disabled() &&
           layouts.getMainOutputChannelSet() == juce::AudioChannelSet::disabled();
#endif
}

juce::AudioProcessorValueTreeState::ParameterLayout apcStepperMainProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"tempo", apcPARAMETER_V1}, "Tempo", 0, 240,
                                                         98));
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"transpose", apcPARAMETER_V1}, "Transpose",
                                                         -24, 24, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"velocityScale", apcPARAMETER_V1},
                                                           "Velocity Scale",
                                                           juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));

    for (int step = 0; step < 8; ++step) {
        for (int trackNr = 0; trackNr < 8; ++trackNr) {
            juce::String parameterID = "step_" + juce::String(step) + "_track_" + juce::String(trackNr);
            layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{parameterID, apcPARAMETER_V1},
                                                                  parameterID, false));
        }
    }

    for (int fatButtonNr = 0; fatButtonNr < 8; ++fatButtonNr) {
        juce::String parameterID = "fatButton_" + juce::String(fatButtonNr);
        layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{parameterID, apcPARAMETER_V1},
                                                              parameterID, false));
    }

    for (int sliderNr = 0; sliderNr < 8; ++sliderNr) {
        juce::String parameterID = "slider_" + juce::String(sliderNr);
        layout.add(std::make_unique<juce::AudioParameterFloat>(
                juce::ParameterID{parameterID, apcPARAMETER_V1},
                parameterID, // Name
                juce::NormalisableRange<float>(0.0f, 1.0f), // Range (min, max)
                0.0f)); // Default value
    }

    return layout;
}

void apcStepperMainProcessor::parameterChanged(const juce::String &parameterID, float newValue) {
    APCLOG("parameter changed: " + parameterID + " = " + std::to_string(newValue));

    midiGrid.updateFromString(parameterID.toStdString(), newValue);

}



/* This creates a MIDI file  */
void apcStepperMainProcessor::exportMIDI(const juce::String &parameterID, float newValue)
{
    // Parse parameter ID (format: "step_X_track_Y")
    juce::StringArray tokens = juce::StringArray::fromTokens(parameterID, "_", "");
    if (tokens.size() != 4 || !tokens[0].equalsIgnoreCase("step") || !tokens[2].equalsIgnoreCase("track"))
        return;

    int stepNum = tokens[1].getIntValue(); // 1-8
    int trackNum = tokens[3].getIntValue(); // 1-8

    // Check input range before decrementing
    if (stepNum < 0 || stepNum > 7 || trackNum < 0 || trackNum > 7)
        return;

    // Convert to 0-based indexing
    stepNum--;
    trackNum--;

    // MIDI note: C-1 (24) for track 8, up to B0 (35) for track 1
    int midiNote = 37 + trackNum; // Invert track order: track 8 = C-1, track 1 = B0
    // Alternatively, if you want C-1 upward: int midiNote = 24 + trackNum;

    // Calculate time in ticks (1/8 note = 48 ticks)
    int tickPosition = stepNum * 48;

    // Remove any existing note at this position for this MIDI note
    for (int i = trackSequence->getNumEvents() - 1; i >= 0; i--) {
        const juce::MidiMessage *msg = &trackSequence->getEventPointer(i)->message;
        if (msg->isNoteOn() &&
            msg->getNoteNumber() == midiNote &&
            juce::approximatelyEqual(msg->getTimeStamp(), (double) tickPosition)) {
            trackSequence->deleteEvent(i, true);
        }
    }

    // If newValue is 1, add a new note
    if (juce::approximatelyEqual(newValue, 1.0f)) {
        juce::MidiMessage noteOn = juce::MidiMessage::noteOn(
                1, // MIDI channel 1
                midiNote, // Note number
                0.8f // Velocity
        );
        noteOn.setTimeStamp(tickPosition);

        juce::MidiMessage noteOff = juce::MidiMessage::noteOff(1, midiNote);
        noteOff.setTimeStamp(tickPosition + 48); // 1/8 note duration

        trackSequence->addEvent(noteOn);
        trackSequence->addEvent(noteOff);
        trackSequence->updateMatchedPairs();
    }
}

void apcStepperMainProcessor::saveMidiFile(const juce::File &file) const {
    juce::FileOutputStream output(file);
    if (output.openedOk()) {
        // Create a fresh MidiFile
        juce::MidiFile newMidiFile;
        newMidiFile.setTicksPerQuarterNote(96);

        // Create a copy of the track sequence and add meta events
        auto newTrackSequence = std::make_unique<juce::MidiMessageSequence>(*trackSequence);

        // Add time signature and tempo if not present
        if (newTrackSequence->getNumEvents() == 0 ||
            !newTrackSequence->getEventPointer(0)->message.isTimeSignatureMetaEvent()) {
            newTrackSequence->addEvent(juce::MidiMessage::timeSignatureMetaEvent(4, 4), 0);
            newTrackSequence->addEvent(juce::MidiMessage::tempoMetaEvent(500000), 0); // 120 BPM
        }

        // Add end of track marker before adding to file
        newTrackSequence->addEvent(juce::MidiMessage::endOfTrack(),
                                   newTrackSequence->getEndTime() + 1);

        // Add the single track
        newMidiFile.addTrack(*newTrackSequence);

        // Write to file
        newMidiFile.writeTo(output);
        output.flush();
    } else {
        APCLOG("Failed to open MIDI file for writing!");
    }
}


/*
 *
 * ProcessBlockMIDI - call this to determine whether or not a step needs to be played
 *
 Notes:

	Key Architectural Considerations

		- Use a fixed-size 2D array or vector for storing step states
		- Each row represents an instrument (MIDI note).
		- Each column represents a step (beat).
		- The values are true (active) or false (inactive).
		- A simple std::array<bool, 8> or std::vector<std::vector<bool>> works well.
		- Keep a transport-aware stepIndex to track playback position
		- This updates every time a new step is triggered, based on the sequencer tempo.
		- Precompute MIDI messages per step for efficient processing
		- Instead of calculating everything in processBlock(), prepare a precomputed schedule of MIDI events.
		- Use MIDI buffers to queue and send messages efficiently
		- JUCE’s MidiBuffer is ideal for handling MIDI event timing.

 */
void apcStepperMainProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {
#if JUCE_STANDALONE_APPLICATION
    buffer.clear(); // Pass-through or clear audio for standalone
#endif

    juce::MidiBuffer processedMidi;
    int transposeAmount = transposeParam->get();
    float velocityScale = velocityScaleParam->get();
    bool doTranspose = false;

// NOTE

    auto numSamples = buffer.getNumSamples();
    auto playHead = getPlayHead();

// TEMPO

    // Process MIDI Clock and other messages
    for (const auto metadata: midiMessages) {
        auto message = metadata.getMessage();
        auto timeStamp = metadata.samplePosition;

        if (message.isMidiClock()) // Check for MIDI Clock (0xF8)
        {
            if (sampleRate > 0.0) // Ensure sample rate is valid
            {
                double currentTime = static_cast<double>(timeStamp) / sampleRate;

                if (lastClockSample >= 0) // Not the first clock
                {
                    double interval = currentTime - lastClockTime;
                    accumulatedInterval += interval;
                    clockCount++;

                    // Update tempo every 24 clocks (1 quarter note) for stability
                    if (clockCount >= clocksPerQuarterNote) {
                        // Calculate average interval per clock
                        double avgIntervalPerClock = accumulatedInterval / clockCount;
                        // Tempo (BPM) = 60 / (time per quarter note)
                        // Time per quarter note = avgIntervalPerClock * 24
                        double quarterNoteTime = avgIntervalPerClock * clocksPerQuarterNote;
                        int calculatedTempo = juce::roundToInt(60.0 / quarterNoteTime);

                        // Clamp to parameter range (0-240)
                        calculatedTempo = juce::jlimit(0, 240, calculatedTempo);

                        // Update tempo parameter if different
                        if (tempoParam && *tempoParam != calculatedTempo) {
                            tempoParam->beginChangeGesture();
                            *tempoParam = calculatedTempo;
                            tempoParam->endChangeGesture();
                            DBG("MIDI Clock detected - Tempo set to: " << calculatedTempo);
                        }

                        // Reset accumulators
                        accumulatedInterval = 0.0;
                        clockCount = 0;
                    }
                }

                lastClockSample = timeStamp;
                lastClockTime = currentTime;
            }
        } else if (doTranspose) {
            if (message.isNoteOn()) {
                int newNote = juce::jlimit(0, 127, message.getNoteNumber() + transposeAmount);
                float newVelocity = juce::jlimit(0.0f, 1.0f, message.getFloatVelocity() * velocityScale);
                message = juce::MidiMessage::noteOn(message.getChannel(), newNote, newVelocity);
            } else if (message.isNoteOff()) {
                int newNote = juce::jlimit(0, 127, message.getNoteNumber() + transposeAmount);
                message = juce::MidiMessage::noteOff(message.getChannel(), newNote);
            }

            processedMidi.addEvent(message, timeStamp);
        }
    }


    if (playHead != nullptr) {
        juce::AudioPlayHead::CurrentPositionInfo posInfo;
        if (playHead->getCurrentPosition(posInfo)) {
            if (posInfo.isPlaying) {
                double ppqPosition = posInfo.ppqPosition; // Current position in beats

                // Compute which step we're on
                int newStepIndex = static_cast<int>(std::floor(ppqPosition / ppqPerStep)) % numSteps;

                if (newStepIndex != currentStepIndex) // Only update when a new step starts
                {
                    currentStepIndex = newStepIndex;

                    // Process active steps for this column
                    for (int instrument = 0; instrument < numInstruments; ++instrument) {
                        // [instrument][currentStepIndex]) // If step is active
                        if (midiGrid.getInstAt(currentStepIndex, instrument))
                        {
                            int midiNote = 36 + instrument; // Map row index to MIDI notes (C1 and up)
                            midiMessages.addEvent(juce::MidiMessage::noteOn(1, midiNote, (juce::uint8) 100), 0);
                        }
                    }
                }
            }
        }
    }

    // Clear old MIDI notes after a short delay (MIDI Note Off)
    int noteOffTime = static_cast<int>(numSamples * 0.9); // 90% into the block
    for (int instrument = 0; instrument < numInstruments; ++instrument) {
        if (midiGrid.getInstAt(currentStepIndex, instrument)) {
            int midiNote = 36 + instrument;
            midiMessages.addEvent(juce::MidiMessage::noteOff(1, midiNote), noteOffTime);
        }
    }

    midiMessages.swapWith(processedMidi);
}


void apcStepperMainProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    this->sampleRate = sampleRate;

    midiSlider.resize(numSteps);
    midiFatButton.resize(numSteps);

    currentStepIndex = -1;
    lastClockSample = -1;
    lastClockTime = 0.0;
    accumulatedInterval = 0.0;
    clockCount = 0;
    scrollOffset = 0;
    pageOffset = 0;

    APCLOG("apcStepperMainProcessor: prepareToPlay");
}

void apcStepperMainProcessor::releaseResources() {
}

int apcStepperMainProcessor::mapRowColumnToNote(int instrument, int /*step*/) {
    // Simple mapping: base note (C1 = 36) + instrument offset + transpose
    return 36 + instrument; // Removed step multiplication as it was creating large jumps
}

juce::AudioProcessorEditor *apcStepperMainProcessor::createEditor() {
    return new apcStepperMainEditor(*this);
}

void apcStepperMainProcessor::getStateInformation(juce::MemoryBlock &) {
}

void apcStepperMainProcessor::setStateInformation(const void *, int) {
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
    return new apcStepperMainProcessor();
}
