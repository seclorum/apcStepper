// Created by Tom Peak Walcher on 05.03.25.
//
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "apcStepperTrack.h"
#include "ToggleSquare.h"
#include "ToggleSquareShift.h"

class apcStepperMainProcessor;

class apcControlPanel : public juce::AudioProcessorEditor {
public:
    static constexpr int rows = 8;
    static constexpr int cols = 8;

    apcControlPanel(apcStepperMainProcessor &p)
        : AudioProcessorEditor(&p), processor(p) {
        auto imageInputStreamPlay = std::make_unique<juce::MemoryInputStream>(
            BinaryData::playcircle_svg, BinaryData::playcircle_svgSize, false);
        playButton = juce::PNGImageFormat().decodeImage(*imageInputStreamPlay);

        auto imageInputStreamStop = std::make_unique<juce::MemoryInputStream>(
            BinaryData::stopcircle_svg, BinaryData::stopcircle_svgSize, false);
        stopButton = juce::PNGImageFormat().decodeImage(*imageInputStreamStop);

        auto imageInputStreamShift = std::make_unique<juce::MemoryInputStream>(
            BinaryData::shift_svg, BinaryData::shift_svgSize, false);
        shiftButton = juce::PNGImageFormat().decodeImage(*imageInputStreamShift);

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

        // Initialize toggle buttons with images
        playToggleButton = std::make_unique<ToggleSquare>(juce::Colours::green, juce::Colours::darkgreen, playButton);
        addAndMakeVisible(playToggleButton.get());

        stopToggleButton = std::make_unique<ToggleSquare>(juce::Colours::red, juce::Colours::darkgrey, stopButton);
        addAndMakeVisible(stopToggleButton.get());

        shiftToggleButton = std::make_unique<ToggleSquare>(juce::Colours::blue, juce::Colours::orange, shiftButton);
        addAndMakeVisible(shiftToggleButton.get());
    }

    void resized() {
        auto bounds = getLocalBounds();

        juce::FlexBox mainFlexBox;
        juce::FlexBox rightPanel;
        juce::FlexBox gridFlexBox;

        juce::FlexBox downButtons;
        mainFlexBox.flexDirection = juce::FlexBox::Direction::row;
        mainFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        mainFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;

        rightPanel.flexDirection = juce::FlexBox::Direction::column;
        rightPanel.justifyContent = juce::FlexBox::JustifyContent::center;
        rightPanel.alignItems = juce::FlexBox::AlignItems::stretch;


        downButtons.flexDirection = juce::FlexBox::Direction::column; // Changed to row
        downButtons.justifyContent = juce::FlexBox::JustifyContent::spaceAround; // Changed to spaceAround
        downButtons.alignItems = juce::FlexBox::AlignItems::center;
        //Keep align items center so the buttons are vertically centered.


        for (auto &btn: rowButtons) {
            rightPanel.items.add(juce::FlexItem(*btn).withFlex(1).withMargin(juce::FlexItem::Margin(4)));
        }


        gridFlexBox.flexDirection = juce::FlexBox::Direction::row;

        gridFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;

        gridFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;


        for (auto &column: columns) {
            gridFlexBox.items.add(juce::FlexItem(*column).withFlex(1).withHeight(bounds.getHeight()));
        }






        mainFlexBox.items.add(juce::FlexItem(gridFlexBox).withFlex(4));

        mainFlexBox.items.add(juce::FlexItem(rightContainer).withFlex(1));


        mainFlexBox.performLayout(bounds.toFloat());
        // Add dummy component


        // Calculate square size using the dummy component's bounds

    }

private:
    apcStepperMainProcessor &processor;
    juce::OwnedArray<ToggleSquare> rowButtons;
    std::unique_ptr<ToggleSquare> playToggleButton;
    std::unique_ptr<ToggleSquare> stopToggleButton;
    std::unique_ptr<ToggleSquare> shiftToggleButton;
    juce::OwnedArray<apcStepperTrack> columns;
    juce::Image playButton;
    juce::Image stopButton;
    juce::Image shiftButton;
    juce::Component emptySpace;
    ToggleSquareShift rightContainer;
};
