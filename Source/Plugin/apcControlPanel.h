//
// Created by Tom Peak Walcher on 05.03.25.
//
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

#include "BinaryData.h"

class apcStepperMainProcessor;

class apcControlPanel : public juce::AudioProcessorEditor {
public:
    static constexpr int rows = 8;
    static constexpr int cols = 8;

    apcControlPanel(apcStepperMainProcessor &p)
        : AudioProcessorEditor(&p), processor(p), grid(p) {

        for (auto *col: columns) {
            addAndMakeVisible(col);
        }

        addAndMakeVisible(emptySpace);

        for (int i = 0; i < cols; ++i) {
            auto rowButton = std::make_unique<ToggleSquare>(juce::Colours::grey, juce::Colours::blue, juce::Image());
            rowButtons.add(std::move(rowButton));
        }

        for (auto *btn: rowButtons) {
            addAndMakeVisible(btn);
        }

        addAndMakeVisible(downPanel);


    }

void resized() override {
    auto bounds = getLocalBounds();
    int containerDownHeight = 100;  // Adjust this to fit your needs
    juce::Rectangle<int> containerBounds = bounds.removeFromBottom(containerDownHeight);
    juce::Rectangle<int> mainBounds = bounds;  // Space for grid + right panel

    // Create main layout boxes
    juce::FlexBox mainFlexBox;
    juce::FlexBox containerFlex;
    juce::FlexBox containerFlexBox;
    juce::FlexBox containerDownFlex;
    juce::FlexBox gridFlexBox;

    // Set up main layout properties
    mainFlexBox.flexDirection = juce::FlexBox::Direction::row;
    mainFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    mainFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;

    juce::FlexBox rightPanel;
    rightPanel.flexDirection = juce::FlexBox::Direction::column;
    rightPanel.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;

    // Add row buttons
    for (int i = 0; i < rows; ++i) {
        rightPanel.items.add(juce::FlexItem(*rowButtons[i]).withFlex(1).withMargin(juce::FlexItem::Margin(4)));
    }
    for (int i = 0; i < cols; ++i) {
        containerFlexBox.items.add(juce::FlexItem(*columns[i]).withFlex(1).withMargin(juce::FlexItem::Margin(4)));
    }

    // Add grid and rightPanel to mainFlexBox
    mainFlexBox.items.add(juce::FlexItem(grid).withFlex(4));  // Grid takes up most space
    mainFlexBox.items.add(juce::FlexItem(rightPanel).withFlex(1));  // Right panel takes less space
    // Perform Layouts
    mainFlexBox.performLayout(getLocalBounds().toFloat());// Ensure full height for DownPanel
}


private:
    apcStepperMainProcessor &processor;
    apcStepperGrid grid;
    juce::OwnedArray<ToggleSquare> rowButtons;

    // TODO: this
    class apcStepTrack : public juce::Component {};

    juce::OwnedArray<apcStepTrack> columns;

    DownPanel downPanel;
    juce::Component emptySpace;
};
