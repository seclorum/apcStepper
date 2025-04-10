// Created by Tom Peak Walcher on 05.03.25.
//
#pragma once

#include "Common.h"

#include "trackDeckStep.h"
#include "apcToggleParameterButton.h"
#include "apcRightPanel.h"


class trackDeckMainProcessor;

class apcControlPanel : public juce::AudioProcessorEditor, private juce::Timer {
public:
    static constexpr int rows = 8;
    static constexpr int cols = 8;

    apcControlPanel(trackDeckMainProcessor &p)
        : AudioProcessorEditor(&p), processor(p) {

        // Initialize columns
        for (int i = 0; i < cols; ++i) {
            columns.add(std::make_unique<trackDeckStep>(processor,i));
            addAndMakeVisible(columns.getLast());
        }
        rightContainer = std::make_unique<apcRightPanel>(processor);
        addAndMakeVisible(rightContainer.get());
//        engine = std::make_unique<tracktion_engine::Engine>("MyApp");
//        edit = std::make_unique<tracktion_engine::Edit>(*engine, tracktion_engine::createEmptyEdit(*engine), nullptr);

        // Ensure at least one track exists
//        edit->ensureNumberOfAudioTracks(1);
//        auto track = tracktion_engine::getAudioTracks(*edit)[0];
//        midiClip = track->insertMIDIClip({0.0, 4.0}, nullptr); // Example clip from 0 to 4 beats
//
//        addAndMakeVisible(trackView);

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

        mainFlexBox.items.add(juce::FlexItem(gridFlexBox).withFlex(4).withMargin(8));

        mainFlexBox.items.add(juce::FlexItem(*rightContainer).withFlex(1));


        mainFlexBox.performLayout(bounds.toFloat());
        // Add dummy component

        APCLOG("apcControl Panel initialized.");

        // Calculate square size using the dummy component's bounds

    }
    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colour(0xff041937));
        if (auto drawable = juce::Drawable::createFromImageData(BinaryData::back_svg, BinaryData::back_svgSize)) {
            juce::Rectangle<float> targetBounds = getLocalBounds().toFloat();

            // Get the SVG's intrinsic bounds (viewport-like bounds)
            juce::Rectangle<float> svgBounds = drawable->getDrawableBounds();

            // Calculate the scaling factor to fit the SVG within the target bounds while preserving aspect ratio
            float scaleX = targetBounds.getWidth() / svgBounds.getWidth();
            float scaleY =  targetBounds.getHeight() / svgBounds.getHeight();

            // Calculate the scaled size of the SVG
            float scaledWidth = svgBounds.getWidth() * scaleX;
            float scaledHeight = svgBounds.getHeight() * scaleY;

            // Center the SVG in the target bounds
            juce::Rectangle<float> placedBounds(
                0,
                0,
                scaledWidth,
                scaledHeight
            );

            // Draw the SVG within the calculated bounds
            drawable->drawWithin(g, placedBounds, juce::RectanglePlacement::stretchToFit, 1.0f);
        }
    }

private:
    trackDeckMainProcessor &processor;
    juce::OwnedArray<trackDeckStep> columns;

    juce::Component emptySpace;
     std::unique_ptr<apcRightPanel> rightContainer;
//    std::unique_ptr<tracktion_engine::Engine> engine;
//    std::unique_ptr<tracktion_engine::Edit> edit;
//    tracktion_engine::MidiClip* midiClip = nullptr;
//    TrackView trackView; // Custom component for displaying MIDI tracks
};
