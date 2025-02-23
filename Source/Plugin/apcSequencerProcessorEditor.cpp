//
// Created by Tom Peak Walcher on 21.02.25.
//

#include "apcSequencerProcessorEditor.h"
#include "apcSequencerProcessor.h"


class apcStepperGrid : public juce::AudioProcessorEditor
{

public:
    // Constructor: initializes the editor and attaches UI components to processor parameters.
    apcStepperGrid (apcSequencerProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
    {
            // Create 16x24 grid of toggle buttons
            for (int row = 0; row < 24; ++row)
            {
                for (int col = 0; col < 16; ++col)
                {
                    auto button = std::make_unique<juce::ToggleButton>("Button R" + juce::String(row) + " C" + juce::String(col));
                    button->setBounds(50 + col * 40, 50 + row * 20, 30, 15);
                    button->onClick = [this, row, col]() { toggleGridState(row, col); };
                    addAndMakeVisible(*button);
                    gridButtons.push_back(std::move(button));
                }
        }

        // Add track buttons
        for (int i = 0; i < 8; ++i)
        {
            auto trackButton = std::make_unique<juce::TextButton>("Track Button " + juce::String(i + 1));
            trackButton->setBounds(10, 50 + i * 40, 100, 30);
            trackButton->onClick = [this, i]() { handleTrackButtonPress(i); };
            addAndMakeVisible(*trackButton);
            trackButtons.push_back(std::move(trackButton));
        }

        // Add scene launch buttons
        for (int i = 0; i < 8; ++i)
        {
            auto sceneButton = std::make_unique<juce::TextButton>("Scene Launch " + juce::String(i + 1));
            sceneButton->setBounds(50 + i * 60, 500, 100, 30);
            sceneButton->onClick = [this, i]() { handleSceneButtonPress(i); };
            addAndMakeVisible(*sceneButton);
            sceneButtons.push_back(std::move(sceneButton));
        }
    }


    void toggleGridState(int row, int col)
    {
        int adjustedCol = (col + processor.pageOffset * 8) % 16;
        int adjustedRow = (row + processor.scrollOffset) % 24;
        processor.midiGrid[adjustedCol][adjustedRow] = !processor.midiGrid[adjustedCol][adjustedRow];
    }

    void handleTrackButtonPress(int index)
    {
        switch (index)
        {
            case 4: processor.scrollGridUp(); break;
            case 5: processor.scrollGridDown(); break;
            case 6: processor.jumpPageLeft(); break;
            case 7: processor.jumpPageRight(); break;
        }
    }

    void handleSceneButtonPress(int index)
    {
        juce::Logger::writeToLog("Scene Launch " + juce::String(index + 1) + " pressed");
    }


private:
    apcSequencerProcessor& processor;

    std::vector<std::unique_ptr<juce::ToggleButton>> gridButtons;
    std::vector<std::unique_ptr<juce::TextButton>> trackButtons;
    std::vector<std::unique_ptr<juce::TextButton>> sceneButtons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (apcStepperGrid)

};

apcSequencerProcessorEditor::apcSequencerProcessorEditor(apcSequencerProcessor& p)
    : juce::AudioProcessorEditor(p), processor(p)
{
    setSize(800, 600);

    addAndMakeVisible(apcTabs);

}

apcSequencerProcessorEditor::~apcSequencerProcessorEditor() = default;

void apcSequencerProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void apcSequencerProcessorEditor::resized()
{
    // Adjust layout dynamically if needed
}
