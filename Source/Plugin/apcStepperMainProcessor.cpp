#include "apcStepperMainProcessor.h"
#include "apcStepperMainEditor.h"

static const int apcPARAMETER_V1 = 0x01;

apcStepperMainProcessor::apcStepperMainProcessor()
		: parameters(*this, nullptr, "PARAMETERS",
					 {
							 std::make_unique<juce::AudioParameterInt>(ParameterID{"transpose", apcPARAMETER_V1}, "Transpose", -24, 24, 0),
							 std::make_unique<juce::AudioParameterFloat>(ParameterID{"velocityScale", apcPARAMETER_V1}, "Velocity Scale",
																		 juce::NormalisableRange<float>(0.0f, 2.0f,
																										0.01f, 1.0f),
																		 1.0f)
					 }) {
	// Assign parameter pointers after initialization
	transposeParam = dynamic_cast<juce::AudioParameterInt *>(parameters.getParameter("transpose"));
	velocityScaleParam = dynamic_cast<juce::AudioParameterFloat *>(parameters.getParameter("velocityScale"));

	jassert(transposeParam && velocityScaleParam);

	// Set parameter state property
	parameters.state.setProperty("parameterVersion", parameterVersion, nullptr);
}

// !J!  TODO: figure out bus layout
#if 0
apcStepperMainProcessor::apcStepperMainProcessor()
	: juce::AudioProcessor(BusesProperties()
		.withOutput("MIDI Out", juce::AudioChannelSet::disabled(), true))  // ✅ No audio processing
{
}
#endif

void apcStepperMainProcessor::initializeParameters() {
	using namespace juce;

	// Retrieve parameters from the parameter tree
	transposeParam = dynamic_cast<AudioParameterInt *>(parameters.getParameter("transpose"));
	velocityScaleParam = dynamic_cast<AudioParameterFloat *>(parameters.getParameter("velocityScale"));

	jassert(transposeParam && velocityScaleParam);

	// Set default values with notification
	if (transposeParam)
		transposeParam->operator=(0); // Default to 0 with notification

	if (velocityScaleParam)
		velocityScaleParam->operator=(1.0f); // Default to 1.0f with notification

	// Set parameter state property
	parameters.state.setProperty("parameterVersion", parameterVersion, nullptr);
}


apcStepperMainProcessor::~apcStepperMainProcessor() = default;

void apcStepperMainProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
	juce::ignoreUnused(sampleRate, samplesPerBlock);

	for (auto &row: midiGrid) {
		row.fill(false);
	}
	scrollOffset = 0;
	pageOffset = 0;
}

void apcStepperMainProcessor::releaseResources() {}


void apcStepperMainProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {
	juce::ScopedTryLock lock(midiMutex);
	if (lock.isLocked()) {
		midiMessages.addEvents(incomingMidiBuffer, 0, buffer.getNumSamples(), 0);
		incomingMidiBuffer.clear();
	}

	juce::MidiBuffer processedMidi;
	int transposeAmount = transposeParam->get();
	float velocityScale = velocityScaleParam->get();

	for (const auto metadata: midiMessages) {
		auto message = metadata.getMessage();
		auto timeStamp = metadata.samplePosition;

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

bool apcStepperMainProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
	return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::disabled();
}

// This function is required for JUCE plugins!
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
	return new apcStepperMainProcessor();
}

