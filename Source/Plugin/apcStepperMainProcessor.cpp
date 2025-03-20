#include <vector>
#include <regex>
#include "apcStepperMainProcessor.h"
#include "apcStepperMainEditor.h"

static const int apcPARAMETER_V1 = 1;

apcStepperMainProcessor::apcStepperMainProcessor()
    : juce::AudioProcessor(getBusesProperties()),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    // Initialize parameters
    tempoParam = dynamic_cast<juce::AudioParameterInt*>(parameters.getParameter("tempo"));
    transposeParam = dynamic_cast<juce::AudioParameterInt*>(parameters.getParameter("transpose"));
    velocityScaleParam = dynamic_cast<juce::AudioParameterFloat*>(parameters.getParameter("velocityScale"));

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

    // Set initial values
    *tempoParam = 98;
    *transposeParam = 0;
    *velocityScaleParam = 1.0f;

    parameters.state.setProperty("parameterVersion", parameterVersion, nullptr);

    // Initialize midiGrid
    midiGrid.resize(numInstruments);
    for (auto& row : midiGrid) {
        row.resize(numSteps, false);
    }
}

apcStepperMainProcessor::~apcStepperMainProcessor() = default;

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

juce::AudioProcessorValueTreeState::ParameterLayout apcStepperMainProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"tempo", apcPARAMETER_V1}, "Tempo", 0, 240, 98));
    layout.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"transpose", apcPARAMETER_V1}, "Transpose", -24, 24, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"velocityScale", apcPARAMETER_V1}, "Velocity Scale",
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));

    for (int step = 0; step < 8; ++step) {
        for (int trackNr = 0; trackNr < 8; ++trackNr) {
            juce::String parameterID = "step_" + juce::String(step) + "_track_" + juce::String(trackNr);
            layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{parameterID, apcPARAMETER_V1},
                parameterID, false));
        }
    }
    return layout;
}

void apcStepperMainProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    std::string input = parameterID.toStdString();
    std::regex pattern(R"(step_(\d+)_track_(\d+))");
    std::smatch matches;

    if (std::regex_match(input, matches, pattern) && matches.size() == 3) {
        int step = std::stoi(matches[1].str());
        int track = std::stoi(matches[2].str());
        if (step < numSteps && track < numInstruments) {
            midiGrid[track][step] = (newValue > 0.5f);
            APCLOG("step_" + std::to_string(step) + "_" + std::to_string(track) + " = " + std::to_string(newValue));
        }
    }
    // Handle other parameter changes
    else if (parameterID == "tempo") {
        APCLOG("Processor: Tempo changed to: " + juce::String(newValue));
    }
    else if (parameterID == "transpose") {
        APCLOG("Processor: Transpose changed to: " + juce::String(newValue));
    }
    else if (parameterID == "velocityScale") {
        APCLOG("Processor: Velocity Scale changed to: " + juce::String(newValue));
    }
}

void apcStepperMainProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
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
                            midiMessages.addEvent(juce::MidiMessage::noteOff(1, midiNote + transposeParam->get()), noteOffTime);
                        }
                    }
                }
            }
        }
    }
}

void apcStepperMainProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate;
    midiGrid.resize(numInstruments);
    for (auto& row : midiGrid) {
        row.resize(numSteps, false);
    }

    currentStepIndex = -1;
    lastClockSample = -1;
    lastClockTime = 0.0;
    accumulatedInterval = 0.0;
    clockCount = 0;
    scrollOffset = 0;
    pageOffset = 0;

    APCLOG("apcStepperMainProcessor: prepareToPlay");
}

void apcStepperMainProcessor::releaseResources() {}

int apcStepperMainProcessor::mapRowColumnToNote(int instrument, int /*step*/)
{
    // Simple mapping: base note (C1 = 36) + instrument offset + transpose
    return 36 + instrument;  // Removed step multiplication as it was creating large jumps
}

juce::AudioProcessorEditor* apcStepperMainProcessor::createEditor()
{
    return new apcStepperMainEditor(*this);
}

void apcStepperMainProcessor::getStateInformation(juce::MemoryBlock&) {}
void apcStepperMainProcessor::setStateInformation(const void*, int) {}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new apcStepperMainProcessor();
}