#pragma once

#include "Common.h"

#include <vector>
#include <string>

class StepGrid {
private:
    // Flat vector for the grid, size will be width * height
    std::vector<int> grid;

    // Vector of vectors to store instruction indices per step
    std::vector<std::vector<int>> stepToInsts;

    // Current dimensions
    int width;
    int height;

public:
    // Constructor with custom dimensions
    StepGrid(int w = 8, int h = 8) : width(w), height(h) {
        // Initialize grid with -1 (no instruction)
        grid.resize(w * h, -1);
        // Initialize step lookup for each step (0 to width-1)
        stepToInsts.resize(w);
    }

    // Resize the grid, preserving data where possible
    void resize(int newWidth, int newHeight) {
        // Create new grid
        std::vector<int> newGrid(newWidth * newHeight, -1);
        // Copy existing data
        int minWidth = std::min(width, newWidth);
        int minHeight = std::min(height, newHeight);
        for (int x = 0; x < minWidth; x++) {
            for (int y = 0; y < minHeight; y++) {
                newGrid[x * newHeight + y] = grid[x * height + y];
            }
        }
        // Update grid
        grid = std::move(newGrid);

        // Resize stepToInsts
        stepToInsts.resize(newWidth);
        // Clear any stepToInsts beyond old width
        for (int i = width; i < newWidth; i++) {
            stepToInsts[i].clear();
        }
        // Update dimensions
        width = newWidth;
        height = newHeight;
    }

    // Convert 2D coordinates to 1D index
    inline int getIndex(int x, int y) const {
        return x * height + y;
    }

    // Add an instruction to the grid
    void addInstruction(int step, int inst, const std::string& name) {
        // Validate bounds
        if (step >= 0 && step < width && inst >= 0 && inst < height) {
            int index = getIndex(step, inst);
            grid[index] = inst;
            // Add to step lookup if not already present
            if (std::find(stepToInsts[step].begin(), stepToInsts[step].end(), inst)
                == stepToInsts[step].end()) {
                stepToInsts[step].push_back(inst);
            }
        }
    }

    // Get all instructions for a given step
    inline const std::vector<int>& getInstructions(int step) const {
        // Return reference to vector; empty vector if step invalid
        static const std::vector<int> empty;
        return (step >= 0 && step < width) ? stepToInsts[step] : empty;
    }

    // Get instruction at specific position
    inline int getInstAt(int x, int y) const {
        // Return -1 if out of bounds
        return (x >= 0 && x < width && y >= 0 && y < height) ?
               grid[getIndex(x, y)] : -1;
    }

    // Add an instruction to the grid with bool value
    void addInstruction(int step, int inst, const std::string& name, bool value = true) {
        // Validate bounds
        if (step >= 0 && step < width && inst >= 0 && inst < height) {
            int index = getIndex(step, inst);
            grid[index] = value;
            // Add to step lookup if not already present and value is true
            if (value && std::find(stepToInsts[step].begin(), stepToInsts[step].end(), inst)
                         == stepToInsts[step].end()) {
                stepToInsts[step].push_back(inst);
            }
                // Remove from step lookup if value is false
            else if (!value) {
                auto it = std::find(stepToInsts[step].begin(), stepToInsts[step].end(), inst);
                if (it != stepToInsts[step].end()) {
                    stepToInsts[step].erase(it);
                }
            }
        }
    }

    // TODO: get rid of the "step_X_track_Y" bollocks, replace it with "01", "02", "03" .. "21" .. "22" .. etc.
    // Update grid value from "stepXinstY" string
    void updateFromString(const std::string& name, bool value) {
        // Check if string matches expected format !J! TODO : Fix this bollocks
        if (name.length() < 8 || name.substr(0, 5) != "step_" ||
                name.substr(8, 13) != "track_") {
            return;  // Invalid format
        }

        // Extract step (x) and inst (y) numbers
        int step = 0;
        int inst = 0;
        size_t pos = 4;  // After "step"

        // Parse step number
        while (pos < name.length() && name[pos] != 'i') {
            if (name[pos] < '0' || name[pos] > '9') return;
            step = step * 10 + (name[pos] - '0');
            pos++;
        }

        // Move past "inst"
        pos += 4;

        // Parse inst number
        while (pos < name.length()) {
            if (name[pos] < '0' || name[pos] > '9') return;
            inst = inst * 10 + (name[pos] - '0');
            pos++;
        }

        // Update grid with value
        addInstruction(step, inst, name, value);
    }

    // Get instruction from "stepXinstY" string
    int getInstFromString(const std::string& name) const {
        // Check if string matches expected format
        if (name.length() < 8 || name.substr(0, 4) != "step" || name[5] != 'i' ||
            name[6] != 'n' || name[7] != 's' || name[8] != 't') {
            return -1;  // Invalid format
        }

        // Extract step (x) and inst (y) numbers
        int step = 0;
        int inst = 0;
        size_t pos = 4;  // After "step"

        // Parse step number
        while (pos < name.length() && name[pos] != 'i') {
            if (name[pos] < '0' || name[pos] > '9') return -1;
            step = step * 10 + (name[pos] - '0');
            pos++;
        }

        // Move past "inst"
        pos += 4;

        // Parse inst number
        while (pos < name.length()) {
            if (name[pos] < '0' || name[pos] > '9') return -1;
            inst = inst * 10 + (name[pos] - '0');
            pos++;
        }

        // Return instruction if within bounds
        return getInstAt(step, inst);
    }

    // Get current dimensions
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};

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

    StepGrid midiGrid{8, 8};  // Initial 8x8 grid

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
