#include <vector>
#include <regex>
#include "apcStepperMainProcessor.h"
#include "apcStepperMainEditor.h"

static const int apcPARAMETER_V1 = 1;

apcStepperMainProcessor::apcStepperMainProcessor()
    : juce::AudioProcessor(getBusesProperties()),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout()) {
    // Initialize parameters
    tempoParam = dynamic_cast<juce::AudioParameterInt *>(parameters.getParameter("tempo"));
    transposeParam = dynamic_cast<juce::AudioParameterInt *>(parameters.getParameter("transpose"));
    velocityScaleParam = dynamic_cast<juce::AudioParameterFloat *>(parameters.getParameter("velocityScale"));

    if (!tempoParam || !transposeParam || !velocityScaleParam) {
        juce::Logger::writeToLog("Error: Failed to initialize parameters!");
        return;
    }

    // Add parameter listeners
    parameters.addParameterListener("tempo", this);
    parameters.addParameterListener("transpose", this);
    parameters.addParameterListener("velocityScale", this);

    // Initialize step track parameters
    for (int step = 0; step < 8; step++) {
        for (int trackNr = 0; trackNr < 8; trackNr++) {
            juce::String parameterID = "step_" + juce::String(step) + "_track_" + juce::String(trackNr);
            parameters.addParameterListener(parameterID, this);
        }
    }

    // Initialize FatButton  parameters
    for (int fatButtonNr = 0; fatButtonNr < 8; fatButtonNr++) {
        juce::String parameterID = "fatButton_" + juce::String(fatButtonNr);
        parameters.addParameterListener(parameterID, this);
    }

    // Initialize slider parameters
    for (int sliderNr = 0; sliderNr < 8; sliderNr++) {
        juce::String parameterID = "slider_" + juce::String(sliderNr);
        parameters.addParameterListener(parameterID, this);
    }

    // Set initial values
    *tempoParam = 98;
    *transposeParam = 0;
    *velocityScaleParam = 1.0f;

    parameters.state.setProperty("parameterVersion", parameterVersion, nullptr);

    // Initialize midiGrid
    midiGrid.resize(numInstruments);
    for (auto &row: midiGrid) {
        row.resize(numSteps, false);
    }

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
    int midiNote = 37  +  trackNum; // Invert track order: track 8 = C-1, track 1 = B0
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

void apcStepperMainProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {
#if JUCE_STANDALONE_APPLICATION
    buffer.clear();
#endif

    juce::ScopedLock lock(midiMutex);
    midiMessages.addEvents(incomingMidiBuffer, 0, buffer.getNumSamples(), 0);
    incomingMidiBuffer.clear();

    auto playHead = getPlayHead();
    if (playHead != nullptr) {
        juce::AudioPlayHead::CurrentPositionInfo posInfo;
        if (playHead->getCurrentPosition(posInfo)) {
            if (posInfo.isPlaying) {
                double ppqPosition = posInfo.ppqPosition;
                int newStepIndex = static_cast<int>(std::floor(ppqPosition / ppqPerStep)) % numSteps;

                if (newStepIndex != currentStepIndex) {
                    currentStepIndex = newStepIndex;

                    for (int instrument = 0; instrument < numInstruments; ++instrument) {
                        if (midiGrid[instrument][currentStepIndex]) {
                            int midiNote = mapRowColumnToNote(instrument, currentStepIndex);
                            float velocity = velocityScaleParam->get() * 100.0f;
                            midiMessages.addEvent(juce::MidiMessage::noteOn(1, midiNote + transposeParam->get(),
                                                                            juce::jlimit(0.0f, 127.0f, velocity)), 0);

                            // Schedule note off
                            int noteOffTime = buffer.getNumSamples() * 0.9;
                            midiMessages.addEvent(juce::MidiMessage::noteOff(1, midiNote + transposeParam->get()),
                                                  noteOffTime);
                        }
                    }
                }
            }
        }
    }
}

void apcStepperMainProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    this->sampleRate = sampleRate;
    midiGrid.resize(numInstruments);
    for (auto &row: midiGrid) {
        row.resize(numSteps, false);
    }

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

juce::AudioProcessor * JUCE_CALLTYPE createPluginFilter() {
    return new apcStepperMainProcessor();
}
