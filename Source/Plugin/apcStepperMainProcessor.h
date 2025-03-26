#pragma once


#include "Common.h"

#include <vector>


#include "apcResizeableArray.h"
#include <unordered_map>
#include <iostream>



// Declaration and usage example:
/*
StepGrid stepGrid(8, 8);  // Initial 8x8 grid
stepGrid.addInstruction(0, 0, "step0inst0");
int inst = stepGrid.getInstFromString("step0inst0");  // Returns 0
int invalid = stepGrid.getInstFromString("step9inst9");  // Returns -1
*/



// Forward declaration to avoid circular dependency
class apcStepperMainEditor;


class apcStepperMainProcessor : public juce::AudioProcessor,
								public juce::AudioProcessorValueTreeState::Listener
{
public:

    apcStepperMainProcessor();
    ~apcStepperMainProcessor() override;
	juce::AudioProcessorValueTreeState parameters;

	std::unique_ptr<juce::MidiOutput> midiOutput = nullptr;
	juce::StringArray midiOutputDevices;
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

	juce::AudioProcessorValueTreeState& getParameters() { return parameters; }

	void parameterChanged(const juce::String& parameterID, float newValue) override;

    void exportMIDI(const juce::String &parameterID, float newValue);

    void saveMidiFile(const juce::File &file) const;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    void releaseResources() override;

	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

	// !J! Merge processBlock and processBlockTEMPO
	void processBlockTEMPO(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);


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


    void toggleButton(juce::String toogleState);


    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override {}
    const juce::String getProgramName(int index) override { return {}; }
    void changeProgramName(int index, const juce::String& newName) override {}

    void sendMidiMessage(const juce::MidiMessage& message);

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

	// For MIDI step calculations:
	static const int numSteps = 8;
	static const int numInstruments = 8;
	int currentStepIndex;
	const int ppqPerStep = 1; // !J! TODO: Adjust this

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
    // this gets resized!
//    std::array<std::array<bool, 1>, 1> midiGrid{};
//	std::vector<std::vector<bool>> midiGrid;

    apcResizableArray<bool> midiGrid{8, 8};  // Initial 8x8 grid

	std::vector<std::vector<float>> midiSlider;
	std::vector<std::vector<bool>> midiFatButton;

	juce::AudioParameterInt* tempoParam;
	juce::AudioParameterInt* transposeParam;
	juce::AudioParameterFloat* velocityScaleParam;
	juce::MidiFile midiFile;
	std::unique_ptr<juce::MidiMessageSequence> trackSequence;


	static juce::AudioProcessor::BusesProperties getBusesProperties();

    int mapRowColumnToNote(int row, int column);

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void initializeParameters();


    static constexpr int parameterVersion = 1;  // Versioning for future compatibility
	juce::AudioProcessorValueTreeState::ParameterLayout layout;
//    juce::AudioParameterInt* step_1_track_1_Param = nullptr;
	std::vector<std::unique_ptr<juce::AudioParameterBool>> stepTrackButtonGroup;

    juce::MidiBuffer incomingMidiBuffer;
    juce::CriticalSection midiMutex;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(apcStepperMainProcessor)
};


