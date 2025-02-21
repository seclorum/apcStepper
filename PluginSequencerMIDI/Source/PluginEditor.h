#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include <memory>

class apcSequencerProcessor;

class apcSequencerProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit apcSequencerProcessorEditor(apcSequencerProcessor& processor);
    ~apcSequencerProcessorEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void toggleGridState(int row, int col);
    void handleTrackButtonPress(int index);
    void handleSceneButtonPress(int index);

    apcSequencerProcessor& processor;
    std::vector<std::unique_ptr<juce::ToggleButton>> gridButtons;
    std::vector<std::unique_ptr<juce::TextButton>> trackButtons;
    std::vector<std::unique_ptr<juce::TextButton>> sceneButtons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(apcSequencerProcessorEditor)
};
