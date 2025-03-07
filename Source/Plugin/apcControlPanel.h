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

        // Initialize columns
        for (int i = 0; i < cols; ++i) {
            columns.add(std::make_unique<apcStepperTrack>(processor));
            addAndMakeVisible(columns.getLast());
        }
        rightContainer = std::make_unique<ToggleSquareShift>();
        addAndMakeVisible(rightContainer.get());
        // Initialize row buttons

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


        //Keep align items center so the buttons are vertically centered.




        gridFlexBox.flexDirection = juce::FlexBox::Direction::row;

        gridFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;

        gridFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;


        for (auto &column: columns) {
            gridFlexBox.items.add(juce::FlexItem(*column).withFlex(1).withHeight(bounds.getHeight()));
        }






        mainFlexBox.items.add(juce::FlexItem(gridFlexBox).withFlex(4));

        mainFlexBox.items.add(juce::FlexItem(*rightContainer).withFlex(1));


        mainFlexBox.performLayout(bounds.toFloat());
        // Add dummy component


        // Calculate square size using the dummy component's bounds

    }

private:
    apcStepperMainProcessor &processor;;
    juce::OwnedArray<apcStepperTrack> columns;

    juce::Component emptySpace;
     std::unique_ptr<ToggleSquareShift> rightContainer;
};
