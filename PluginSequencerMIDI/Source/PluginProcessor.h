#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <array>

// Forward declaration to avoid circular dependency
class apcSequencerProcessorEditor;

class apcSequencerProcessor : public juce::AudioProcessor
{
public:
    apcSequencerProcessor();
    ~apcSequencerProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

    void scrollGridUp();
    void scrollGridDown();
    void jumpPageLeft();
    void jumpPageRight();

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "apcSequencerPlugin"; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return true; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override {}
    const juce::String getProgramName(int index) override { return {}; }
    void changeProgramName(int index, const juce::String& newName) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    std::array<std::array<bool, 24>, 16> midiGrid{};
    int scrollOffset = 0;
    int pageOffset = 0;

private:
    int mapRowColumnToNote(int row, int column);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(apcSequencerProcessor)
};
