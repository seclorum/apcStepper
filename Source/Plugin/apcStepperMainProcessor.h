#pragma once

#include "Common.h"


// Forward declaration to avoid circular dependency
class apcStepperMainEditor;

class apcStepperMainProcessor : public juce::AudioProcessor,
								public juce::AudioProcessorValueTreeState::Listener
{
public:
    apcStepperMainProcessor();
    ~apcStepperMainProcessor() override;

	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

	juce::AudioProcessorValueTreeState& getParameters() { return parameters; }

	void parameterChanged(const juce::String& parameterID, float newValue) override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    void releaseResources() override;

	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

	void scrollGridUp();
    void scrollGridDown();
    void jumpPageLeft();
    void jumpPageRight();

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "apcStepperMainProcessor"; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return true; }
    double getTailLengthSeconds() const override { return 0.0; }

	void setTempo(int newTempo);

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override {}
    const juce::String getProgramName(int index) override { return {}; }
    void changeProgramName(int index, const juce::String& newName) override {}

    void sendMidiMessage(const juce::MidiMessage& message);

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    std::array<std::array<bool, 24>, 16> midiGrid{};
    int scrollOffset = 0;
    int pageOffset = 0;

	//M IDI Clock detection
	double sampleRate = 0.0;              // Store sample rate from prepareToPlay
	int lastClockSample = -1;             // Sample position of the last MIDI Clock
	double lastClockTime = 0.0;           // Time in seconds of the last MIDI Clock
	double accumulatedInterval = 0.0;     // Accumulated intervals for averaging
	int clockCount = 0;                   // Number of clocks since last tempo update
	static constexpr int clocksPerQuarterNote = 24; // MIDI Clock standard


private:
	static juce::AudioProcessor::BusesProperties getBusesProperties();

    int mapRowColumnToNote(int row, int column);
    void initializeParameters();

    juce::AudioProcessorValueTreeState parameters;
    static constexpr int parameterVersion = 1;  // Versioning for future compatibility

	juce::AudioParameterInt* tempoParam = nullptr;
	juce::AudioParameterInt* transposeParam = nullptr;
    juce::AudioParameterFloat* velocityScaleParam = nullptr;
//    juce::AudioParameterInt* step_1_track_1_Param = nullptr;
	std::array<std::unique_ptr<juce::AudioParameterBool>, 8> stepTrackButtonGroup;

    juce::MidiBuffer incomingMidiBuffer;
    juce::CriticalSection midiMutex;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(apcStepperMainProcessor)
};
