#include "apcStepperMainProcessor.h"
#include "apcStepperMainEditor.h"


apcStepperMainProcessor::apcStepperMainProcessor()
        : parameters(*this, nullptr, "PARAMETERS", {})
{
    initializeParameters();
}
// !J!  TODO: figure out bus layout
#if 0
apcStepperMainProcessor::apcStepperMainProcessor()
    : juce::AudioProcessor(BusesProperties()
        .withOutput("MIDI Out", juce::AudioChannelSet::disabled(), true))  // ✅ No audio processing
{
}
#endif

void apcStepperMainProcessor::initializeParameters()
{
    using namespace juce;

    NormalisableRange<float> velocityRange(0.0f, 2.0f, 0.01f);
    velocityRange.setSkewForCentre(1.0f);

    parameters.createAndAddParameter(std::make_unique<AudioParameterInt>("transpose", "Transpose",
                                                                         -24, 24, 0));
    parameters.createAndAddParameter(std::make_unique<AudioParameterFloat>("velocityScale", "Velocity Scale",
                                                                           velocityRange, 1.0f));

    parameters.state.setProperty("parameterVersion", parameterVersion, nullptr);

    transposeParam = dynamic_cast<AudioParameterInt*>(parameters.getParameter("transpose"));
    velocityScaleParam = dynamic_cast<AudioParameterFloat*>(parameters.getParameter("velocityScale"));

    jassert(transposeParam && velocityScaleParam);
}


apcStepperMainProcessor::~apcStepperMainProcessor() = default;

void apcStepperMainProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);

    for (auto& row : midiGrid)
    {
        row.fill(false);
    }
    scrollOffset = 0;
    pageOffset = 0;
}

void apcStepperMainProcessor::releaseResources() {}


void apcStepperMainProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedTryLock lock(midiMutex);
    if (lock.isLocked())
    {
        midiMessages.addEvents(incomingMidiBuffer, 0, buffer.getNumSamples(), 0);
        incomingMidiBuffer.clear();
    }

    juce::MidiBuffer processedMidi;
    int transposeAmount = transposeParam->get();
    float velocityScale = velocityScaleParam->get();

    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();
        auto timeStamp = metadata.samplePosition;

        if (message.isNoteOn())
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



void apcStepperMainProcessor::sendMidiMessage(const juce::MidiMessage& message)
{
    juce::ScopedLock lock(midiMutex);
    incomingMidiBuffer.addEvent(message, 0);
}

void apcStepperMainProcessor::scrollGridUp() { scrollOffset = std::max(0, scrollOffset - 1); }
void apcStepperMainProcessor::scrollGridDown() { scrollOffset = std::min(23, scrollOffset + 1); }
void apcStepperMainProcessor::jumpPageLeft() { pageOffset = std::max(0, pageOffset - 1); }
void apcStepperMainProcessor::jumpPageRight() { pageOffset = std::min(1, pageOffset + 1); }

juce::AudioProcessorEditor* apcStepperMainProcessor::createEditor()
{
    return new apcStepperMainEditor(*this);
}

void apcStepperMainProcessor::getStateInformation(juce::MemoryBlock&) {}
void apcStepperMainProcessor::setStateInformation(const void*, int) {}

int apcStepperMainProcessor::mapRowColumnToNote(int row, int column)
{
    return 36 + (row + column * 24);
}

bool apcStepperMainProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::disabled();
}

// This function is required for JUCE plugins!
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new apcStepperMainProcessor();
}

