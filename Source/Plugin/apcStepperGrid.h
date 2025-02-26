//
// Created by Jay Vaughan on 26.02.25.
//


// !J! Just include all the JUCE things.  Makes it easier to navigate... but slower to build
#include <juce_analytics/juce_analytics.h>
#include <juce_animation/juce_animation.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_box2d/juce_box2d.h>
#include <juce_core/juce_core.h>
#include <juce_cryptography/juce_cryptography.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_javascript/juce_javascript.h>
#include <juce_midi_ci/juce_midi_ci.h>
#include <juce_opengl/juce_opengl.h>
#include <juce_osc/juce_osc.h>
#include <juce_product_unlocking/juce_product_unlocking.h>
#include <juce_video/juce_video.h>


#ifndef APCSTEPPER_APCSTEPPERGRID_H
#define APCSTEPPER_APCSTEPPERGRID_H

class apcStepperGrid : public juce::AudioProcessorEditor
{
public:
    // Constructor: initializes the editor and attaches UI components to processor parameters.
    apcStepperGrid(apcStepperMainProcessor& p)
            : AudioProcessorEditor(&p), processor(p)
    {
        setSize(800, 600); // Ensure the editor has a defined size

        // Create 16x24 grid of toggle buttons
        for (int row = 0; row < 24; ++row)
        {
            for (int col = 0; col < 16; ++col)
            {
                auto button = std::make_unique<juce::ToggleButton>(
                        "Button R" + juce::String(row) + " C" + juce::String(col));
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
            case 4:
                processor.scrollGridUp();
                break;
            case 5:
                processor.scrollGridDown();
                break;
            case 6:
                processor.jumpPageLeft();
                break;
            case 7:
                processor.jumpPageRight();
                break;
        }
    }

    void handleSceneButtonPress(int index)
    {
        juce::Logger::writeToLog("Scene Launch " + juce::String(index + 1) + " pressed");
    }

private:
    apcStepperMainProcessor& processor;

    std::vector<std::unique_ptr<juce::ToggleButton>> gridButtons;
    std::vector<std::unique_ptr<juce::TextButton>> trackButtons;
    std::vector<std::unique_ptr<juce::TextButton>> sceneButtons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(apcStepperGrid)
};
#endif //APCSTEPPER_APCSTEPPERGRID_H
