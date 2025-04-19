// Created by Tom Peak Walcher on 05.03.25.
//
#pragma once

#include "Common.h"
#include <memory> // For std::unique_ptr
#include "tdEditBar.h" // Ensure tdEditBar is included first
#include "trackDeckStep.h"
#include "apcToggleParameterButton.h"
#include "apcRightPanel.h"

class trackDeckMainProcessor;

class apcControlPanel : public juce::AudioProcessorEditor, private juce::Timer {
public:
    static constexpr int rows = 8;
    static constexpr int cols = 16;

    apcControlPanel(trackDeckMainProcessor &p)
        : AudioProcessorEditor(&p), processor(p)
    {
        // Initialize columns
        for (int i = 0; i < cols; ++i) {
            columns.add(std::make_unique<trackDeckStep>(processor, i));
            addAndMakeVisible(columns.getLast());
        }

        // Initialize rightContainer and tdEditBar
        rightContainer.reset(new apcRightPanel(processor));
        tdEditBar.reset(new class tdEditBar()); // Should work with full definition
        addAndMakeVisible(rightContainer.get());
        addAndMakeVisible(tdEditBar.get());

        APCLOG("Main initialized...");
        //startTimer(25); // Uncomment if needed
    }

    void timerCallback() override {
        // Empty for now
    }

    void resized() override {
        auto bounds = getLocalBounds();

        juce::FlexBox mainFlexBox;
        juce::FlexBox stepperFlexBox;
        juce::FlexBox gridFlexBox;

        mainFlexBox.flexDirection = juce::FlexBox::Direction::column;
        mainFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        mainFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;

        stepperFlexBox.flexDirection = juce::FlexBox::Direction::row;
        stepperFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        stepperFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;

        gridFlexBox.flexDirection = juce::FlexBox::Direction::row;
        gridFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        gridFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;

        for (auto &column : columns) {
            gridFlexBox.items.add(juce::FlexItem(*column).withFlex(1).withHeight(bounds.getHeight() * 0.8f));
        }

        stepperFlexBox.items.add(juce::FlexItem(gridFlexBox).withFlex(4).withMargin(8));
        stepperFlexBox.items.add(juce::FlexItem(*rightContainer.get()).withFlex(1));

        // Add tdEditBar to the layout with explicit height
        mainFlexBox.items.add(juce::FlexItem(*tdEditBar.get()).withFlex(1).withHeight(bounds.getHeight() * 0.2f));
        mainFlexBox.items.add(juce::FlexItem(stepperFlexBox).withFlex(4));

        mainFlexBox.performLayout(bounds.toFloat());

        APCLOG("apcControl Panel resized.");
    }

    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colour(0xff000000));
        // SVG painting disabled to ensure tdEditBar visibility
        /*
        if (auto drawable = juce::Drawable::createFromImageData(BinaryData::back_svg, BinaryData::back_svgSize)) {
            juce::Rectangle<float> targetBounds = getLocalBounds().toFloat();
            juce::Rectangle<float> svgBounds = drawable->getDrawableBounds();
            float scaleX = targetBounds.getWidth() / svgBounds.getWidth();
            float scaleY = targetBounds.getHeight() / svgBounds.getHeight();
            float scaledWidth = svgBounds.getWidth() * scaleX;
            float scaledHeight = svgBounds.getHeight() * scaleY;
            juce::Rectangle<float> placedBounds(0, 0, scaledWidth, scaledHeight);
            drawable->drawWithin(g, placedBounds, juce::RectanglePlacement::stretchToFit, 1.0f);
        }
        */
    }

private:
    trackDeckMainProcessor &processor;
    juce::OwnedArray<trackDeckStep> columns;
    juce::Component emptySpace;
    std::unique_ptr<apcRightPanel> rightContainer;
    std::unique_ptr<tdEditBar> tdEditBar; // Correct declaration
};

