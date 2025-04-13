#pragma once

#include "Common.h"
#include "apcResizeableArray.h"
#include "apcNoteObject.h"

class trackDeckMainEditor;

class trackDeckMainProcessor : public juce::AudioProcessor,
                               public juce::AudioProcessorValueTreeState::Listener
{
public:
    trackDeckMainProcessor();
    ~trackDeckMainProcessor() override;

    juce::AudioProcessorValueTreeState parameters{*this, nullptr, "PARAMETERS", createParameterLayout()};
    std::unique_ptr<juce::MidiOutput> midiOutput;
    juce::StringArray midiOutputDevices;
    int currentSelect = 0;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    juce::AudioProcessorValueTreeState& getParameters() { return parameters; }
    std::string addLeadingZeros(int number);

    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void exportMIDI(const juce::String& parameterID, float newValue);
    void saveMidiFile(const juce::File& file) const;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void handleMidiClock(int timeStamp);
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

    void scrollGridUp() { scrollOffset = jmax(0, scrollOffset - 1); redrawGrid();}
    void scrollGridDown() { scrollOffset = jmin(numSteps - 8, scrollOffset + 1); redrawGrid();}
    void jumpPageLeft() { pageOffset = jmax(0, pageOffset - 8); }
    void jumpPageRight() { pageOffset = jmin(numSteps - 8, pageOffset + 8); }
    void redrawGrid(){ for (int step = 0; step < numSteps; ++step) {
        for (int track = 0; track < 8; ++track) {
            auto parameterID = "s" + addLeadingZeros(step) + "t" + addLeadingZeros(track);
            parameters.getParameter(parameterID)->setValueNotifyingHost(midiGrid.at(step,track+scrollOffset).noteOn);
            controlGrid.at(step,track) = midiGrid.at(step,track+scrollOffset).noteOn;

        }
    }
        for (int instrument = 0; instrument < 8; ++instrument) {

            auto parameterID = "i" + addLeadingZeros(instrument);
            parameters.getParameter(parameterID)->setValueNotifyingHost(instrument+scrollOffset);
        }
    }

    int getPreviousStep(int currentStepIndex);

    // !J! Merge processBlock and processBlockTEMPO
	void processBlockTEMPO(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);




    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "trackDeck"; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return true; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    static const int numSteps = 16;
    static const int numInstruments = 16;
    int currentMIDIStep = -1;
    const float ppqPerStep = 1.0/4; // !J! TODO: put this in a GUI

    int scrollOffset = 0;
    int pageOffset = 0;

    double sampleRate = 0.0;
    int lastClockSample = -1;
    double lastClockTime = 0.0;
    double accumulatedInterval = 0.0;
    int clockCount = 0;
    static constexpr int clocksPerQuarterNote = 24;

private:
    apcResizeableArray<apcNoteObject> midiGrid{numSteps, numInstruments, apcNoteObject()};
    apcResizeableArray<int> controlGrid{numSteps, numInstruments, 0};
    juce::AudioParameterInt* tempoParam;
    juce::AudioParameterInt* transposeParam;
    juce::AudioParameterFloat* velocityScaleParam;
    juce::MidiFile midiFile;
    std::unique_ptr<juce::MidiMessageSequence> trackSequence;

    static juce::AudioProcessor::BusesProperties getBusesProperties();
    int mapRowColumnToNote(int row, int column);
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::MidiBuffer incomingMidiBuffer;
    juce::CriticalSection midiMutex;

    static constexpr int parameterVersion = 1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(trackDeckMainProcessor)
};
