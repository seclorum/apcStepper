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
#include "tdSongList.h"

class trackDeckMainProcessor;

class apcControlPanel : public juce::AudioProcessorEditor,
                       private juce::Timer,
                       private juce::RangedAudioParameter::Listener,
                        public juce::DragAndDropContainer
{
public:
    static constexpr int rows = 8;
    static constexpr int cols = 16;

    apcControlPanel(trackDeckMainProcessor& p)
        : AudioProcessorEditor(&p), processor(p)
    {
        // Initialize columns
        for (int i = 0; i < rows; ++i) {
            rowContainer.add(std::make_unique<tdTrack>(processor, i));
            addAndMakeVisible(rowContainer.getLast());
        }
        controllerBar = std::make_unique<tdTrackControllerBar>(processor, 99);
        addAndMakeVisible(*controllerBar);
        songList = std::make_unique<CustomListComponent>();
        addAndMakeVisible(*songList);

        // Initialize rightContainer and tdEditBar
        tdEditBar.reset(new class tdEditBar()); // Should work with full definition
        addAndMakeVisible(tdEditBar.get());

        current_attachment = processor.getParameters().getParameter("currentTrack");
        if (current_attachment)
            current_attachment->addListener(this);

        APCLOG("Main initialized...");
        // startTimer(25); // Uncomment if needed
    }

    ~apcControlPanel() override
    {
        if (current_attachment)
            current_attachment->removeListener(this);
    }

    // Implement pure virtual functions from RangedAudioParameter::Listener
    void parameterValueChanged(int parameterIndex, float newValue) override
    {
        // Handle parameter changes (e.g., update UI)
        if (current_attachment && current_attachment->getParameterIndex() == parameterIndex) {
            currentTrack = static_cast<int>(newValue); // Update currentTrack based on parameter
            resized(); // Trigger layout update to reflect new track selection
        }
    }

    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override
    {
        // Handle gesture changes (optional, can be empty if not needed)
    }


    void timerCallback() override
    {
        // Empty for now
    }

    void resized() override
    {
        auto bounds = getLocalBounds();

        juce::FlexBox mainFlexBox;
        juce::FlexBox gridFlexBox;
        juce::FlexBox controlFlexBox;

        mainFlexBox.flexDirection = juce::FlexBox::Direction::column;
        mainFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        mainFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;

        gridFlexBox.flexDirection = juce::FlexBox::Direction::column;
        gridFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        gridFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;

        controlFlexBox.flexDirection = juce::FlexBox::Direction::row;
        controlFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        controlFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;

        int i = 0;
        for (auto& row : rowContainer) {
            gridFlexBox.items.add(juce::FlexItem(*row).withFlex(1));
            if (i == currentTrack) {
                gridFlexBox.items.add(juce::FlexItem(*controllerBar).withFlex(3));
            }
            ++i;
        }

        // Add tdEditBar to the layout with explicit height
        mainFlexBox.items.add(juce::FlexItem(*tdEditBar.get()).withFlex(3));
        mainFlexBox.items.add(juce::FlexItem(gridFlexBox).withFlex(7));

        controlFlexBox.items.add(juce::FlexItem(mainFlexBox).withFlex(4));
        controlFlexBox.items.add(juce::FlexItem(*songList).withFlex(1));

        controlFlexBox.performLayout(bounds.toFloat());

        APCLOG("apcControlPanel resized.");
    }

    void paint(juce::Graphics& g) override
    {
        auto totalBounds = getLocalBounds().toFloat();
        g.fillAll(juce::Colour(0xff000000));
        juce::Colour darkestgrey = juce::Colour::fromFloatRGBA(0.2f, 0.2f, 0.2f, 0.5f);
        juce::Colour darkergrey = juce::Colour::fromFloatRGBA(0.4f, 0.4f, 0.4f, 0.5f);

        const int numRects = 4;
        const float margin = 8.0f;
        float rectWidth = ((totalBounds.getWidth() - 16) * 0.8f / numRects);
        float startX = totalBounds.getX();
        float rectHeight = totalBounds.getHeight();

        for (int i = 0; i < numRects; ++i) {
            juce::Colour currentColour = (i % 2 == 0) ? darkergrey : darkestgrey;
            g.setColour(currentColour);
            float rectX = i * rectWidth + 8;
            juce::Rectangle<float> rect(rectX, totalBounds.getY(), rectWidth, rectHeight);
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
    trackDeckMainProcessor& processor;
    juce::OwnedArray<tdTrack> rowContainer;
    juce::Component emptySpace;
    std::unique_ptr<tdTrackControllerBar> controllerBar;
    std::unique_ptr<CustomListComponent> songList;
    std::unique_ptr<apcRightPanel> rightContainer;
    int currentTrack = 8;
    std::unique_ptr<tdEditBar> tdEditBar;
    juce::RangedAudioParameter* current_attachment = nullptr;

    // Example: Add muteButton if needed
    juce::TextButton muteButton {"Mute"};
    // Add ButtonAttachment if needed
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> muteAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(apcControlPanel)
};