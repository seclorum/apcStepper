// Created by Tom Peak Walcher on 05.03.25.
//
#pragma once

#include "Common.h"
#include <memory> // For std::unique_ptr
#include "tdEditBar.h" // Ensure tdEditBar is included first
#include "tdTrackControllerBar.h"
#include "apcToggleParameterButton.h"
#include "apcRightPanel.h"
#include "tdTrack.h"
class trackDeckMainProcessor;

class apcControlPanel : public juce::AudioProcessorEditor, private juce::Timer {
public:
    static constexpr int rows = 8;
    static constexpr int cols = 16;

    apcControlPanel(trackDeckMainProcessor &p)
        : AudioProcessorEditor(&p), processor(p)
    {
        // Initialize columns
        for (int i = 0; i < rows; ++i) {
            rowContainer.add(std::make_unique<tdTrack>(processor, i));
            addAndMakeVisible(rowContainer.getLast());
        }
        controllerBar = std::make_unique<tdTrackControllerBar>(processor, 99);

        addAndMakeVisible(*controllerBar);

        // Initialize rightContainer and tdEditBar
        tdEditBar.reset(new class tdEditBar()); // Should work with full definition

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


        gridFlexBox.flexDirection = juce::FlexBox::Direction::column;
        gridFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        gridFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;
        int i = 0;
        for (auto &row : rowContainer) {
            gridFlexBox.items.add(juce::FlexItem(*row).withFlex(1));
            if (i==8) gridFlexBox.items.add(juce::FlexItem(*controllerBar).withFlex(3));
            ++i;
        }




        // Add tdEditBar to the layout with explicit height
        mainFlexBox.items.add(juce::FlexItem(*tdEditBar.get()).withFlex(3));
        mainFlexBox.items.add(juce::FlexItem(gridFlexBox).withFlex(7));

        mainFlexBox.performLayout(bounds.toFloat());

        APCLOG("apcControl Panel resized.");
    }

    void paint(juce::Graphics& g) override {
        auto totalBounds = getLocalBounds().toFloat();
        g.fillAll(juce::Colour(0xff000000));
        juce::Colour darkestgrey = juce::Colour::fromFloatRGBA(0.2f, 0.2f, .2f, 0.5f);       // Blue with 50% alpha
        juce::Colour darkergrey = juce::Colour::fromFloatRGBA(0.4f, 0.4f, 0.4f, 0.5f);  // Light Blue with 50% alpha

        // Number of rectangles and margin.
        const int numRects = 4;
        const float margin = 8.0f; // Adjust for desired margin

        // Calculate the width of each rectangle, accounting for the margin.
        float rectWidth = ((totalBounds.getWidth()-16)*0.8f / numRects);

        // Calculate the starting X position, to take into account the left margin
        float startX = totalBounds.getX();
        // Calculate the height of each rectangle (full height).
        float rectHeight = totalBounds.getHeight();

        // Loop through and draw the rectangles.
        for (int i = 0; i < numRects; ++i)
        {
            // Alternate colors.
            juce::Colour currentColour = (i % 2 == 0) ? darkergrey : darkestgrey;
            g.setColour(currentColour);

            // Calculate the x position of the current rectangle.
            float rectX =  i * rectWidth + 8;

            // Create the rectangle.
            juce::Rectangle<float> rect(rectX, totalBounds.getY(), rectWidth, rectHeight);

            // Fill the rectangle.
            g.fillRect(rect);
        }

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

    }

private:
    trackDeckMainProcessor &processor;
    juce::OwnedArray<tdTrack> rowContainer;
    juce::Component emptySpace;
    std::unique_ptr<tdTrackControllerBar> controllerBar;
    std::unique_ptr<apcRightPanel> rightContainer;
    std::unique_ptr<tdEditBar> tdEditBar; // Correct declaration
};

