//
// Created by Tom Peak Walcher on 05.03.25.
//
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "apcStepperTrack.h"
#include "ToggleSquare.h"

class apcStepperMainProcessor;

class apcControlPanel : public juce::AudioProcessorEditor {
public:
    static constexpr int rows = 8;
    static constexpr int cols = 8;

    apcControlPanel(apcStepperMainProcessor& p)
        : AudioProcessorEditor(&p), processor(p) {

        // Initialize columns
        for (int i = 0; i < cols; ++i) {
            columns.add(std::make_unique<apcStepperTrack>(processor));
            addAndMakeVisible(columns.getLast());
        }

        // Initialize row buttons
        for (int i = 0; i < rows; ++i) {
            rowButtons.add(std::make_unique<ToggleSquare>(juce::Colours::grey, juce::Colours::blue, juce::Image()));
            addAndMakeVisible(rowButtons.getLast());
        }

        addAndMakeVisible(emptySpace);
        setSize(600, 400);  // Default size
    }

    void resized() override {
        auto bounds = getLocalBounds();
         // Adjust this based on design needs


        juce::FlexBox mainFlexBox;
        juce::FlexBox rightPanel;
        juce::FlexBox gridFlexBox;

        mainFlexBox.flexDirection = juce::FlexBox::Direction::row;
        mainFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        mainFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;

        rightPanel.flexDirection = juce::FlexBox::Direction::column;
        rightPanel.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        rightPanel.alignItems = juce::FlexBox::AlignItems::stretch; // Ensure full height

        for (auto& btn : rowButtons) {
            rightPanel.items.add(juce::FlexItem(*btn).withFlex(1).withMargin(juce::FlexItem::Margin(4)));
        }

        gridFlexBox.flexDirection = juce::FlexBox::Direction::row;
        gridFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        gridFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;

        for (auto& column : columns) {
            gridFlexBox.items.add(juce::FlexItem(*column).withFlex(1).withHeight(bounds.getHeight()));
        }

        mainFlexBox.items.add(juce::FlexItem(gridFlexBox).withFlex(4));
        mainFlexBox.items.add(juce::FlexItem(rightPanel).withFlex(1));

        mainFlexBox.performLayout(bounds.toFloat());
    }

private:
    apcStepperMainProcessor& processor;
    juce::OwnedArray<ToggleSquare> rowButtons;
    juce::OwnedArray<apcStepperTrack> columns;

    juce::Component emptySpace;
};
