#include "apcStepperMainProcessor.h"
#include "apcStepperMainEditor.h"

apcStepperMainProcessor::apcStepperMainProcessor()
    : AudioProcessor(getBusesProperties())
{
    tempoParam = dynamic_cast<juce::AudioParameterInt*>(parameters.getParameter("tempo"));
    transposeParam = dynamic_cast<juce::AudioParameterInt*>(parameters.getParameter("transpose"));
    velocityScaleParam = dynamic_cast<juce::AudioParameterFloat*>(parameters.getParameter("velocityScale"));

    if (!tempoParam || !transposeParam || !velocityScaleParam)
    {
        APCLOG("Error: Failed to initialize parameters!");
        return;
    }

    *tempoParam = 98;
    *transposeParam = 0;
    *velocityScaleParam = 1.0f;

    // Fixed: Undefined variables numSteps and numInstruments
    int numSteps = 8;        // Defaulting to 8
    int numInstruments = 8;  // Defaulting to 8

    // Initialize step track parameters
    for (int step = 0; step < numSteps; step++) {
        for (int trackNr = 0; trackNr < numInstruments; trackNr++) { // numSteps was not declared, now fixed
            std::string parameterID = "s" + addLeadingZeros(step) + "t" + addLeadingZeros(trackNr);
            parameters.addParameterListener(parameterID, this);

            midiGrid.assignName(parameterID,step,trackNr);

        }
        std::string parameterID = "c" + addLeadingZeros(step);
        parameters.addParameterListener(parameterID, this);
    }

    parameters.state.setProperty("parameterVersion", parameterVersion, nullptr);
    midiFile.setTicksPerQuarterNote(96);
    trackSequence = std::make_unique<juce::MidiMessageSequence>();
    midiFile.addTrack(*trackSequence);
}

apcStepperMainProcessor::~apcStepperMainProcessor() = default;

std::string apcStepperMainProcessor::addLeadingZeros(int number)
{
    std::ostringstream ss;
    ss << std::setw(3) << std::setfill('0') << number;
    return ss.str();
}

juce::AudioProcessor::BusesProperties apcStepperMainProcessor::getBusesProperties()
{
#if JUCE_STANDALONE_APPLICATION
    return BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true);
#else
    return BusesProperties()
        .withOutput("MIDI Out", juce::AudioChannelSet::disabled(), true);
#endif
}

bool apcStepperMainProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
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

void apcStepperMainProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    APCLOG("Parameter changed: " + parameterID + " = " + String(newValue));
    if (midiGrid.hasName(parameterID.toStdString()))
        midiGrid.at(parameterID.toStdString()) = (newValue > 0.5f) ? 1 : 0;
}

juce::AudioProcessorValueTreeState::ParameterLayout apcStepperMainProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"tempo", parameterVersion}, "Tempo", 0, 240, 98));
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"transpose", parameterVersion}, "Transpose", -24, 24, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"velocityScale", parameterVersion}, "Velocity Scale", juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));


    for (int step = 0; step < numSteps; ++step) {
        for (int track = 0; track < numInstruments; ++track) {
            juce::String paramID = "s" + addLeadingZeros(step) + "t" + addLeadingZeros(track);
            layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{paramID, parameterVersion}, paramID, false));
        }
        juce::String paramID = "c" + addLeadingZeros(step);
        layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{paramID, parameterVersion}, paramID, false));
    }
    return layout;
}

void apcStepperMainProcessor::exportMIDI(const juce::String& parameterID, float newValue)
{
    auto tokens = juce::StringArray::fromTokens(parameterID, "st", "");
    if (tokens.size() != 3) return;
    int step = tokens[1].getIntValue();
    int track = tokens[2].getIntValue();
    if (step < 0 || step >= numSteps || track < 0 || track >= numInstruments) return;

    int midiNote = mapRowColumnToNote(track, step);
    double tickPosition = static_cast<double>(step * 48);

    for (int i = trackSequence->getNumEvents() - 1; i >= 0; --i)
    {
        auto* msg = &trackSequence->getEventPointer(i)->message;
        if (msg->isNoteOn() && msg->getNoteNumber() == midiNote && std::abs(msg->getTimeStamp() - tickPosition) < 1e-6)
            trackSequence->deleteEvent(i, true);
    }

    if (newValue > 0.5f)
    {
        trackSequence->addEvent(juce::MidiMessage::noteOn(1, midiNote, 0.8f), tickPosition);
        trackSequence->addEvent(juce::MidiMessage::noteOff(1, midiNote), tickPosition + 48);
        trackSequence->updateMatchedPairs();
    }
}

void apcStepperMainProcessor::saveMidiFile(const juce::File& file) const
{
    juce::FileOutputStream output(file);
    if (output.openedOk())
    {
        juce::MidiFile newMidiFile;
        newMidiFile.setTicksPerQuarterNote(96);
        auto newTrack = std::make_unique<juce::MidiMessageSequence>(*trackSequence);
        newTrack->addEvent(juce::MidiMessage::timeSignatureMetaEvent(4, 4), 0);
        newTrack->addEvent(juce::MidiMessage::tempoMetaEvent(500000), 0);
        newTrack->addEvent(juce::MidiMessage::endOfTrack(), newTrack->getEndTime() + 1);
        newMidiFile.addTrack(*newTrack);
        newMidiFile.writeTo(output);
        output.flush();
    }
    else
        APCLOG("Failed to save MIDI file!");
}

void apcStepperMainProcessor::handleMidiClock(int timeStamp)
{
    if (sampleRate <= 0.0) return;
    double currentTime = static_cast<double>(timeStamp) / sampleRate;
    if (lastClockSample >= 0)
    {
        double interval = currentTime - lastClockTime;
        accumulatedInterval += interval;
        clockCount++;
        if (clockCount >= clocksPerQuarterNote)
        {
            double avgInterval = accumulatedInterval / clockCount;
            int calculatedTempo = juce::roundToInt(60.0 / (avgInterval * clocksPerQuarterNote));
            calculatedTempo = juce::jlimit(0, 240, calculatedTempo);
            if (*tempoParam != calculatedTempo)
            {
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

void apcStepperMainProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
#if JUCE_STANDALONE_APPLICATION
    buffer.clear();
#endif
    juce::MidiBuffer processedMidi;
    int transpose = transposeParam->get();
    float velocityScale = velocityScaleParam->get();

    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();
        auto timeStamp = metadata.samplePosition;
        if (msg.isMidiClock())
            handleMidiClock(timeStamp);
        else if (transpose != 0 || velocityScale != 1.0f)
        {
            if (msg.isNoteOn())
            {
                int newNote = juce::jlimit(0, 127, msg.getNoteNumber() + transpose);
                float newVelocity = juce::jlimit(0.0f, 1.0f, msg.getFloatVelocity() * velocityScale);
                processedMidi.addEvent(juce::MidiMessage::noteOn(msg.getChannel(), newNote, newVelocity), timeStamp);
            }
            else if (msg.isNoteOff())
            {
                int newNote = juce::jlimit(0, 127, msg.getNoteNumber() + transpose);
                processedMidi.addEvent(juce::MidiMessage::noteOff(msg.getChannel(), newNote), timeStamp);
            }
            else
                processedMidi.addEvent(msg, timeStamp);
        }
        else
            processedMidi.addEvent(msg, timeStamp);
    }

<<<<<<< HEAD
    if (auto playHead = getPlayHead())
    {
        auto posInfo = playHead->getPosition();
        if (posInfo && posInfo->getIsPlaying())
        {
            int newStep = static_cast<int>(posInfo->getPpqPosition().orFallback(0.0) / ppqPerStep) % numSteps;
            if (newStep != currentMIDIStep)
            {
                currentMIDIStep = newStep;
                APCLOG("Step: " + std::to_string(currentMIDIStep));
                for (int instrument = 0; instrument < numInstruments; ++instrument)
                {
                    if (midiGrid.at(currentMIDIStep, instrument))
                    {
                        int midiNote = mapRowColumnToNote(instrument, currentMIDIStep);
                        processedMidi.addEvent(juce::MidiMessage::noteOn(1, midiNote, 0.8f), 0);
                        processedMidi.addEvent(juce::MidiMessage::noteOff(1, midiNote), buffer.getNumSamples() / 2);
                    }
                }
            }
        }
    }
    midiMessages.swapWith(processedMidi);
=======
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

			if (newStepIndex != currentStepIndex) {
				currentStepIndex = newStepIndex;

				APCLOG("Step: " + std::to_string(currentStepIndex));

				for (int instrument = 0; instrument < numInstruments; ++instrument) {

					std::string paramameterId = "s" + addLeadingZeros(currentStepIndex)+"t"+addLeadingZeros(instrument)+"c";
					parameters.getParameter(paramameterId)->setValueNotifyingHost(true);
					if (midiGrid.at(currentStepIndex, instrument)) {
						int midiNote = 36 + instrument;
						processedMidi.addEvent(juce::MidiMessage::noteOn(1, midiNote, 0.8f), 0);
						processedMidi.addEvent(juce::MidiMessage::noteOff(1, midiNote), buffer.getNumSamples() / 2);
					}
				}
			}
		}
	}

	midiMessages.swapWith(processedMidi);
>>>>>>> 2e1b386 (parameter set and get)
}
int apcStepperMainProcessor::getPreviousStep(int currentStepIndex) {
	// Ensure input is within 1-8 range
	if (currentStepIndex < 1 || currentStepIndex > 8) {
		return -1; // Error case
	}

<<<<<<< HEAD
void apcStepperMainProcessor::prepareToPlay(double sampleRate_, int)
{
    sampleRate = sampleRate_;
    currentMIDIStep = -1;
=======
	// If at 1, return 8; otherwise return one less
	return (currentStepIndex == 1) ? 8 : currentStepIndex - 1;
}
void apcStepperMainProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    this->sampleRate = sampleRate;

    midiSlider.resize(numSteps);
    midiFatButton.resize(numSteps);

    currentStepIndex = -1;
>>>>>>> 2e1b386 (parameter set and get)
    lastClockSample = -1;
    lastClockTime = 0.0;
    accumulatedInterval = 0.0;
    clockCount = 0;
    scrollOffset = 0;
    pageOffset = 0;
    APCLOG("apcStepperMainProcessor: prepareToPlay");
}

void apcStepperMainProcessor::releaseResources() {}

int apcStepperMainProcessor::mapRowColumnToNote(int instrument, int)
{
    return 36 + instrument; // Simple mapping from C1
}

juce::AudioProcessorEditor* apcStepperMainProcessor::createEditor()
{
    return new apcStepperMainEditor(*this);
}

void apcStepperMainProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void apcStepperMainProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState && xmlState->hasTagName(parameters.state.getType()))
        parameters.replaceState(ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new apcStepperMainProcessor();
}
