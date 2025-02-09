#include <JuceHeader.h>
#include <vector>
#include <algorithm>
#include <map>
// #include <ableton/Link.hpp>

//==============================================================================
class apcSequencerProcessorEditor : public juce::AudioProcessorEditor
{
public:
    apcSequencerProcessorEditor(apcSequencerProcessor& p) : juce::AudioProcessorEditor(&p), processor(p)
    {
        setSize(800, 600); // Adjust size for larger grid

        // Create 16x24 grid of toggle buttons
        for (int row = 0; row < 24; ++row)
        {
            for (int col = 0; col < 16; ++col)
            {
                auto button = std::make_unique<juce::ToggleButton>("Button R" + juce::String(row) + " C" + juce::String(col));
                button->setBounds(50 + col * 40, 50 + row * 20, 30, 15);
                button->onClick = [this, row, col]() {
                    toggleGridState(row, col);
                };
                addAndMakeVisible(*button);
                gridButtons.push_back(std::move(button));
            }
        }

        // Add track buttons
        for (int i = 0; i < 8; ++i)
        {
            auto trackButton = std::make_unique<juce::TextButton>("Track Button " + juce::String(i + 1));
            trackButton->setBounds(10, 50 + i * 40, 100, 30);
            trackButton->onClick = [this, i]() {
                handleTrackButtonPress(i);
            };
            addAndMakeVisible(*trackButton);
            trackButtons.push_back(std::move(trackButton));
        }

        // Add scene launch buttons
        for (int i = 0; i < 8; ++i)
        {
            auto sceneButton = std::make_unique<juce::TextButton>("Scene Launch " + juce::String(i + 1));
            sceneButton->setBounds(50 + i * 60, 500, 100, 30);
            sceneButton->onClick = [this, i]() {
                handleSceneButtonPress(i);
            };
            addAndMakeVisible(*sceneButton);
            sceneButtons.push_back(std::move(sceneButton));
        }
    }

    ~apcSequencerProcessorEditor() override {}

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
    }

    void resized() override
    {
        // Adjust layout dynamically if needed
    }

private:
    void toggleGridState(int row, int col)
    {
        int adjustedCol = (col + processor.pageOffset * 8) % 16;
        int adjustedRow = (row + processor.scrollOffset) % 24;
        processor.midiGrid[adjustedCol][adjustedRow] = !processor.midiGrid[adjustedCol][adjustedRow];
    }

    void handleTrackButtonPress(int index)
    {
        if (index == 4) processor.scrollGridUp();
        else if (index == 5) processor.scrollGridDown();
        else if (index == 6) processor.jumpPageLeft();
        else if (index == 7) processor.jumpPageRight();
    }

    void handleSceneButtonPress(int index)
    {
        juce::Logger::writeToLog("Scene Launch " + juce::String(index + 1) + " pressed");
    }

    apcSequencerProcessor& processor;
    std::vector<std::unique_ptr<juce::ToggleButton>> gridButtons;
    std::vector<std::unique_ptr<juce::TextButton>> trackButtons;
    std::vector<std::unique_ptr<juce::TextButton>> sceneButtons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(apcSequencerProcessorEditor)
};

