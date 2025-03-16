#include <vector>

#include "apcStepperMainProcessor.h"



#include "apcStepperMainEditor.h"

static const int apcPARAMETER_V1 = 0x01;



apcStepperMainProcessor::apcStepperMainProcessor()
		: juce::AudioProcessor(getBusesProperties()),  // Use a helper function for bus config
		  parameters(*this, nullptr, "PARAMETERS",createParameterLayout())
{

	std::set<juce::String> addedParameterIDs;
	tempoParam = dynamic_cast<juce::AudioParameterInt*>(parameters.getParameter("tempo"));
	transposeParam = dynamic_cast<juce::AudioParameterInt*>(parameters.getParameter("transpose"));
	velocityScaleParam = dynamic_cast<juce::AudioParameterFloat*>(parameters.getParameter("velocityScale"));




	if (!tempoParam || !transposeParam || !velocityScaleParam) {
		juce::Logger::writeToLog("Error: Failed to initialize parameters!");
		return;
	}

	// not sure if we need to do this any more...
	//parameters.addParameterListener("step1_track1", this);

	parameters.addParameterListener("tempo", this);
	parameters.addParameterListener("transpose", this);
	parameters.addParameterListener("velocityScale", this);
	for (int step = 0; step < 8; step++) {
		for (int trackNr = 0; trackNr < 8; trackNr++) {
			juce::String parameterID = "step_" + std::to_string(step) + "_track_" + std::to_string(trackNr);

			parameters.addParameterListener(ParameterID{parameterID,apcPARAMETER_V1}.getParamID(), this);
		}
	}
	tempoParam->operator=(98);
	transposeParam->operator=(0);

	velocityScaleParam->operator=(1.0f);
	parameters.state.setProperty("parameterVersion", parameterVersion, nullptr);
}

apcStepperMainProcessor::~apcStepperMainProcessor() = default;

juce::AudioProcessor::BusesProperties apcStepperMainProcessor::getBusesProperties()
{
#if JUCE_STANDALONE_APPLICATION
	return juce::AudioProcessor::BusesProperties()
			.withInput("Input", juce::AudioChannelSet::stereo(), true)
			.withOutput("Output", juce::AudioChannelSet::stereo(), true);
#else
	return juce::AudioProcessor::BusesProperties()
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
juce::AudioProcessorValueTreeState::ParameterLayout apcStepperMainProcessor::createParameterLayout()
{
	juce::AudioProcessorValueTreeState::ParameterLayout layout;

	layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"tempo", 0x01}, "Tempo", 0, 240, 98));
	layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"transpose", 0x01}, "Transpose", -24, 24, 0));
	layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"velocityScale", 0x01}, "Velocity Scale",
															juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f, 1.0f),
															1.0f));

	std::set<std::unique_ptr<juce::AudioParameterBool>> addedParameterIDs;
	stepTrackButtonGroup.resize(200);
	for (int step = 0; step < 8; ++step) {
		for (int trackNr = 0; trackNr < 8; ++trackNr) {
			juce::String parameterID = "step_" + std::to_string(step) + "_track_" + std::to_string(trackNr);


				std::unique_ptr<juce::AudioParameterBool> button =  std::make_unique<juce::AudioParameterBool>(juce::ParameterID{parameterID, apcPARAMETER_V1}.getParamID(), parameterID, false);

				layout.add(std::move(button));

				stepTrackButtonGroup[step*trackNr+trackNr] = std::move(button);
				//APCLOG("Parameter ID just added: " + stepTrackButtonGroup[step*trackNr+trackNr]->getParameterID());

		}
	}
	return layout;
}
void apcStepperMainProcessor::initializeParameters()
{
	using namespace juce;

	// Retrieve parameters
	tempoParam = dynamic_cast<juce::AudioParameterInt*>(parameters.getParameter("tempo"));
	transposeParam = dynamic_cast<juce::AudioParameterInt*>(parameters.getParameter("transpose"));
	velocityScaleParam = dynamic_cast<juce::AudioParameterFloat*>(parameters.getParameter("velocityScale"));
	for (int step = 0; step < 8; step++) {
		for (int trackNr = 0; trackNr < 8; trackNr++) {
			juce::String parameterID = "step_" + std::to_string(step) + "_track_" + std::to_string(trackNr);
			auto stepValue = dynamic_cast<juce::AudioParameterBool*>(parameters.getParameter(parameterID));
			stepTrackButtonGroup[trackNr*step+trackNr]->beginChangeGesture();
			*stepTrackButtonGroup[trackNr*step+trackNr] = stepValue;
			stepTrackButtonGroup[trackNr*step+trackNr]->endChangeGesture();

		}
	}
	jassert(tempoParam && transposeParam && velocityScaleParam);
//
//	if (!tempoParam || !transposeParam || !velocityScaleParam) {
//		return; // Or handle the error appropriately
//	}


	// Set default values with notification
	if (tempoParam)
	{
		tempoParam->beginChangeGesture();
		*tempoParam = 98;
		tempoParam->endChangeGesture();
	}

	if (transposeParam)
	{
		transposeParam->beginChangeGesture();
		*transposeParam = 0;
		transposeParam->endChangeGesture();
	}

	if (velocityScaleParam)
	{
		velocityScaleParam->beginChangeGesture();
		*velocityScaleParam = 1.0f;
		velocityScaleParam->endChangeGesture();
	}

	// Set parameter state property
	parameters.state.setProperty("parameterVersion", parameterVersion, nullptr);
}

void apcStepperMainProcessor::setTempo(int newTempo)
{
	if (tempoParam && *tempoParam != newTempo) // Only update if different
	{
		tempoParam->beginChangeGesture();
		*tempoParam = newTempo;
		tempoParam->endChangeGesture();
	}

    APCLOG(String("Tempo set to" + newTempo));
}


void apcStepperMainProcessor::parameterChanged(const juce::String& parameterID, float newValue){
	for (int step = 0; step < 8; step++) {
		for (int trackNr = 0; trackNr < 8; trackNr++) {


				//stepTrackButtonGroup[trackNr*step+trackNr]->beginChangeGesture();

				//stepTrackButtonGroup[trackNr*step+trackNr]->endChangeGesture();
				//APCLOG("Parameter changed: " + parameterID);}
			///APCLOG(stepTrackButtonGroup[trackNr*step+trackNr]->getParameterID());
		}
	}

	if (parameterID == "tempo")
	{
		APCLOG(String::formatted("Processor: UI changed Tempo to: %d", static_cast<int>(newValue)));
		// Apply any real-time logic if needed
	}
	if (parameterID == "transpose")
	{
//        APCLOG(String::formatted("Processor: UI changed Transpose to: %d",  static_cast<int>(newValue)));
		// Apply any real-time logic if needed
	}
	else if (parameterID == "velocityScale")
	{
//        APCLOG(String::formatted("Processor: UI changed Velocity Scale to:  %f", newValue));
		// Apply any real-time logic if needed
	}
	else {
		APCLOG("Parameter changed: " + parameterID);
	}
}



void apcStepperMainProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {

	juce::ignoreUnused(samplesPerBlock);
	this->sampleRate = sampleRate; // Store sample rate
	for (auto& row : midiGrid) {
		row.fill(false);
	}
	scrollOffset = 0;
	pageOffset = 0;
	lastClockSample = -1;          // Reset clock tracking
	lastClockTime = 0.0;
	accumulatedInterval = 0.0;
	clockCount = 0;

    APCLOG("apcStepperMainProcessor: prepareToPlay");

}

void apcStepperMainProcessor::releaseResources() {}


void apcStepperMainProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
#if JUCE_STANDALONE_APPLICATION
	buffer.clear(); // Pass-through or clear audio for standalone
#endif

	juce::ScopedLock lock(midiMutex);
	midiMessages.addEvents(incomingMidiBuffer, 0, buffer.getNumSamples(), 0);
	incomingMidiBuffer.clear();

	juce::MidiBuffer processedMidi;
	int transposeAmount = transposeParam->get();
	float velocityScale = velocityScaleParam->get();

	// Process MIDI Clock and other messages
	for (const auto metadata : midiMessages)
	{
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
					if (clockCount >= clocksPerQuarterNote)
					{
						// Calculate average interval per clock
						double avgIntervalPerClock = accumulatedInterval / clockCount;
						// Tempo (BPM) = 60 / (time per quarter note)
						// Time per quarter note = avgIntervalPerClock * 24
						double quarterNoteTime = avgIntervalPerClock * clocksPerQuarterNote;
						int calculatedTempo = juce::roundToInt(60.0 / quarterNoteTime);

						// Clamp to parameter range (0-240)
						calculatedTempo = juce::jlimit(0, 240, calculatedTempo);

						// Update tempo parameter if different
						if (tempoParam && *tempoParam != calculatedTempo)
						{
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
		}
		else if (message.isNoteOn())
		{
			int newNote = juce::jlimit(0, 127, message.getNoteNumber() + transposeAmount);
			float newVelocity = juce::jlimit(0.0f, 1.0f, message.getFloatVelocity() * velocityScale);
			message = juce::MidiMessage::noteOn(message.getChannel(), newNote, newVelocity);
		}
		else if (message.isNoteOff())
		{
			int newNote = juce::jlimit(0, 127, message.getNoteNumber() + transposeAmount);
			message = juce::MidiMessage::noteOff(message.getChannel(), newNote);
		}

		processedMidi.addEvent(message, timeStamp);
	}

	midiMessages.swapWith(processedMidi);
}


void apcStepperMainProcessor::sendMidiMessage(const juce::MidiMessage &message) {
	juce::ScopedLock lock(midiMutex);
	incomingMidiBuffer.addEvent(message, 0);
}

void apcStepperMainProcessor::scrollGridUp() { scrollOffset = std::max(0, scrollOffset - 1); }

void apcStepperMainProcessor::scrollGridDown() { scrollOffset = std::min(23, scrollOffset + 1); }

void apcStepperMainProcessor::jumpPageLeft() { pageOffset = std::max(0, pageOffset - 1); }

void apcStepperMainProcessor::jumpPageRight() { pageOffset = std::min(1, pageOffset + 1); }

juce::AudioProcessorEditor *apcStepperMainProcessor::createEditor() {
	return new apcStepperMainEditor(*this);
}

void apcStepperMainProcessor::getStateInformation(juce::MemoryBlock &) {}

void apcStepperMainProcessor::setStateInformation(const void *, int) {}

int apcStepperMainProcessor::mapRowColumnToNote(int row, int column) {
	return 36 + (row + column * 24);
}


// This function is required for JUCE plugins!
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
	return new apcStepperMainProcessor();
}

