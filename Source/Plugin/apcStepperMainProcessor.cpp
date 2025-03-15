#include "apcStepperMainProcessor.h"
#include "apcStepperMainEditor.h"

static const int apcPARAMETER_V1 = 0x01;

apcStepperMainProcessor::apcStepperMainProcessor()
		: juce::AudioProcessor(getBusesProperties()),  // Use a helper function for bus config
		  parameters(*this, nullptr, "PARAMETERS",
					 {
							 std::make_unique<juce::AudioParameterInt>(ParameterID{"tempo", apcPARAMETER_V1}, "Tempo", 0, 240, 98),
							 std::make_unique<juce::AudioParameterInt>(ParameterID{"transpose", apcPARAMETER_V1}, "Transpose", -24, 24, 0),
							 std::make_unique<juce::AudioParameterInt>(ParameterID{"step_1_track_1", apcPARAMETER_V1}, "track",0,1,0),
							 std::make_unique<juce::AudioParameterFloat>(ParameterID{"velocityScale", apcPARAMETER_V1}, "Velocity Scale",
																		 juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f, 1.0f),
																		 1.0f)
					 })
{
	tempoParam = dynamic_cast<juce::AudioParameterInt*>(parameters.getParameter("tempo"));
	transposeParam = dynamic_cast<juce::AudioParameterInt*>(parameters.getParameter("transpose"));
	velocityScaleParam = dynamic_cast<juce::AudioParameterFloat*>(parameters.getParameter("velocityScale"));
	step_1_track_1_Param = dynamic_cast<juce::AudioParameterInt*>(parameters.getParameter("step_1_track_1"));

	if (!tempoParam || !transposeParam || !velocityScaleParam) {
		juce::Logger::writeToLog("Error: Failed to initialize parameters!");
		return;
	}

	parameters.addParameterListener("step_1_track_1", this);
	parameters.addParameterListener("tempo", this);
	parameters.addParameterListener("transpose", this);
	parameters.addParameterListener("velocityScale", this);

	tempoParam->operator=(98);
	transposeParam->operator=(0);
	step_1_track_1_Param->operator=(0);
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

void apcStepperMainProcessor::initializeParameters()
{
	using namespace juce;

	// Retrieve parameters
	tempoParam = dynamic_cast<juce::AudioParameterInt*>(parameters.getParameter("tempo"));
	transposeParam = dynamic_cast<juce::AudioParameterInt*>(parameters.getParameter("transpose"));
	velocityScaleParam = dynamic_cast<juce::AudioParameterFloat*>(parameters.getParameter("velocityScale"));
	step_1_track_1_Param = dynamic_cast<juce::AudioParameterInt*>(parameters.getParameter("step_1_track_1"));

	jassert(tempoParam && transposeParam && velocityScaleParam && step_1_track_1_Param);
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


void apcStepperMainProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
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

