#include <vector>
#include <regex>
#include <iomanip> // Added for std::setw and std::setfill
#include <sstream> // Added for std::stringstream
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

    // Set initial values
    *tempoParam = 98;
    *transposeParam = 0;
    *velocityScaleParam = 1.0f;

    // Fixed: Undefined variables numSteps and numInstruments
    int numSteps = 8;        // Defaulting to 8
    int numInstruments = 8;  // Defaulting to 8

    // Initialize step track parameters
    for (int step = 0; step < 8; step++) {
        for (int trackNr = 0; trackNr < numSteps; trackNr++) { // numSteps was not declared, now fixed
            std::string parameterID = "s" + addLeadingZeros(step) + "t" + addLeadingZeros(trackNr);
            parameters.addParameterListener(parameterID, this);
            midiGrid.assignName(parameterID,step,trackNr);
        }
    }

    // Initialize FatButton parameters
    for (int fatButtonNr = 0; fatButtonNr < numInstruments; fatButtonNr++) { // numInstruments was not declared, now fixed
        juce::String parameterID = "fatButton_" + juce::String(fatButtonNr);
        parameters.addParameterListener(parameterID, this);
    }

    // Initialize slider parameters
    for (int sliderNr = 0; sliderNr < 8; sliderNr++) {
        juce::String parameterID = "slider_" + juce::String(sliderNr);
        parameters.addParameterListener(parameterID, this);
    }

    parameters.state.setProperty("parameterVersion", parameterVersion, nullptr);

    // Initialize MIDI file and sequence
    midiFile.setTicksPerQuarterNote(96); // 1/8 note = 48 ticks

    // Create and add a track
    trackSequence = std::make_unique<juce::MidiMessageSequence>();
    midiFile.addTrack(*trackSequence);

    // Fixed: Incorrect initialization of midiGrid
    midiGrid.resize(8, 8); // Properly initialized as 8x8 grid
}


apcStepperMainProcessor::~apcStepperMainProcessor() = default;





std::string apcStepperMainProcessor::addLeadingZeros(int number) {
    std::stringstream ss;
    ss << std::setw(3) << std::setfill('0') << number;
    return ss.str();
}

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


void apcStepperMainProcessor::parameterChanged(const juce::String &parameterID, float newValue) {
    APCLOG("parameter changed: " + parameterID + " = " + std::to_string(newValue));

    // Fixed: Incorrect method call on midiGrid
    if (midiGrid.at(parameterID.toStdString())) {
        midiGrid.at(parameterID.toStdString()) =  newValue;
        // Proper way to update the grid should be implemented based on logic
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout apcStepperMainProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"tempo", apcPARAMETER_V1}, "Tempo", 0, 240, 98));
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"transpose", apcPARAMETER_V1}, "Transpose", -24, 24, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"velocityScale", apcPARAMETER_V1}, "Velocity Scale", juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));

    // Fixed: Removed invalid method call midiGrid.assignName(parameterID, step, trackNr)
    for (int step = 0; step < 8; ++step) {
        for (int trackNr = 0; trackNr < 8; ++trackNr) {
            juce::String parameterID = "s" + addLeadingZeros(step) + "t" + addLeadingZeros(trackNr);
            layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{parameterID, apcPARAMETER_V1}, parameterID, false));
        }
    }

    for (int fatButtonNr = 0; fatButtonNr < 8; ++fatButtonNr) {
        juce::String parameterID = "fatButton_" + juce::String(fatButtonNr);
        layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{parameterID, apcPARAMETER_V1}, parameterID, false));
    }

    for (int sliderNr = 0; sliderNr < 8; ++sliderNr) {
        juce::String parameterID = "slider_" + juce::String(sliderNr);
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{parameterID, apcPARAMETER_V1}, parameterID, juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    }

    return layout;
}

void apcStepperMainProcessor::exportMIDI(const juce::String &parameterID, float newValue) {
    // Fixed: Incorrect parsing of parameterID tokens
    juce::StringArray tokens = juce::StringArray::fromTokens(parameterID, "_", "");
    if (tokens.size() != 4 || tokens[0] != "step" || tokens[2] != "track")
        return;

    int stepNum = tokens[1].getIntValue();
    int trackNum = tokens[3].getIntValue();

    // Fixed: Incorrect decrement logic
    if (stepNum < 0 || stepNum > 7 || trackNum < 0 || trackNum > 7) return;

    // MIDI note calculation corrected
    int midiNote = 37 + trackNum;

    int tickPosition = stepNum * 48;

    // Remove existing note at this position
    for (int i = trackSequence->getNumEvents() - 1; i >= 0; i--) {
        const juce::MidiMessage *msg = &trackSequence->getEventPointer(i)->message;
        if (msg->isNoteOn() && msg->getNoteNumber() == midiNote && juce::approximatelyEqual(msg->getTimeStamp(), (double)tickPosition)) {
            trackSequence->deleteEvent(i, true);
        }
    }

    // Fixed: Add note only if newValue is 1.0f
    if (juce::approximatelyEqual(newValue, 1.0f)) {
        juce::MidiMessage noteOn = juce::MidiMessage::noteOn(1, midiNote, 0.8f);
        noteOn.setTimeStamp(tickPosition);

        juce::MidiMessage noteOff = juce::MidiMessage::noteOff(1, midiNote);
        noteOff.setTimeStamp(tickPosition + 48);

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


// Add helper method for MIDI clock handling
void apcStepperMainProcessor::handleMidiClock(int timeStamp) {
	if (sampleRate <= 0.0) return;

	double currentTime = static_cast<double>(timeStamp) / sampleRate;

	if (lastClockSample >= 0) {
		double interval = currentTime - lastClockTime;
		accumulatedInterval += interval;
		clockCount++;

		if (clockCount >= clocksPerQuarterNote) {
			double avgIntervalPerClock = accumulatedInterval / clockCount;
			double quarterNoteTime = avgIntervalPerClock * clocksPerQuarterNote;
			int calculatedTempo = juce::roundToInt(60.0 / quarterNoteTime);
			calculatedTempo = juce::jlimit(0, 240, calculatedTempo);

			if (tempoParam && *tempoParam != calculatedTempo) {
				tempoParam->beginChangeGesture();
				*tempoParam = calculatedTempo;
				tempoParam->endChangeGesture();
			}

			accumulatedInterval = 0.0;
			clockCount = 0;
		}
	}

	lastClockSample = timeStamp;
	lastClockTime = currentTime;
}


void apcStepperMainProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
#if JUCE_STANDALONE_APPLICATION
	buffer.clear();
#endif

	juce::MidiBuffer processedMidi;
	int transposeAmount = transposeParam->get();
	float velocityScale = velocityScaleParam->get();

	// Process incoming MIDI
	for (const auto metadata : midiMessages) {
		auto message = metadata.getMessage();
		auto timeStamp = metadata.samplePosition;

		if (message.isMidiClock()) {
			handleMidiClock(timeStamp);
		} else if (transposeAmount != 0 || velocityScale != 1.0f) {
			if (message.isNoteOn()) {
				int newNote = juce::jlimit(0, 127, message.getNoteNumber() + transposeAmount);
				float newVelocity = juce::jlimit(0.0f, 1.0f, message.getFloatVelocity() * velocityScale);
				processedMidi.addEvent(juce::MidiMessage::noteOn(message.getChannel(), newNote, newVelocity), timeStamp);
			} else if (message.isNoteOff()) {
				int newNote = juce::jlimit(0, 127, message.getNoteNumber() + transposeAmount);
				processedMidi.addEvent(juce::MidiMessage::noteOff(message.getChannel(), newNote), timeStamp);
			} else {
				processedMidi.addEvent(message, timeStamp);
			}
		} else {
			processedMidi.addEvent(message, timeStamp);
		}
	}

	// Handle sequencer
	if (auto playHead = getPlayHead()) {
		juce::AudioPlayHead::CurrentPositionInfo posInfo;
		if (playHead->getCurrentPosition(posInfo) && posInfo.isPlaying) {
			int newStepIndex = static_cast<int>(std::floor(posInfo.ppqPosition / ppqPerStep)) % numSteps;

			if (newStepIndex != currentMIDIStep) {
                currentMIDIStep = newStepIndex;

				APCLOG("Step: " + std::to_string(currentMIDIStep));

				for (int instrument = 0; instrument < numInstruments; ++instrument) {
					if (midiGrid.at(currentMIDIStep, instrument)) {
						int midiNote = 36 + instrument;
						processedMidi.addEvent(juce::MidiMessage::noteOn(1, midiNote, 0.8f), 0);
						processedMidi.addEvent(juce::MidiMessage::noteOff(1, midiNote), buffer.getNumSamples() / 2);
					}
				}
			}
		}
	}

	midiMessages.swapWith(processedMidi);
}

void apcStepperMainProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    this->sampleRate = sampleRate;

    midiSlider.resize(numSteps);
    midiFatButton.resize(numSteps);

    currentMIDIStep = 0;
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
