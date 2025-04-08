// Created by Tom Peak Walcher on 05.03.25.
//
#pragma once

#include "Common.h"

#include "apcStepperStep.h"
#include "apcToggleParameterButton.h"
#include "apcRightPanel.h"


class apcStepperMainProcessor;

class apcControlPanel : public juce::AudioProcessorEditor, private juce::Timer {
public:
    static constexpr int rows = 8;
    static constexpr int cols = 8;

    apcControlPanel(apcStepperMainProcessor &p)
        : AudioProcessorEditor(&p), processor(p) {

        // Initialize columns
        for (int i = 0; i < cols; ++i) {
            columns.add(std::make_unique<apcStepperStep>(processor,i));
            addAndMakeVisible(columns.getLast());
        }
        rightContainer = std::make_unique<apcRightPanel>(processor);
        addAndMakeVisible(rightContainer.get());
        APCLOG("Main initialized...");
        //startTimer(25); // !J!
    }

    void timerCallback() override {


    };


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

        APCLOG("apcControl Panel initialized.");

        // Calculate square size using the dummy component's bounds

    }

private:
    apcStepperMainProcessor &processor;
    juce::OwnedArray<apcStepperStep> columns;

    juce::Component emptySpace;
     std::unique_ptr<apcRightPanel> rightContainer;
};
