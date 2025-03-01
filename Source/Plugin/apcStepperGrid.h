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

#include "BinaryData.h"

#define APCSTEPPER_APCSTEPPERGRID_H
#define APCSTEPPER_APCSTEPPERGRID_H

class apcStepperMainProcessor;

class ToggleSquare : public juce::TextButton {
public:
    ToggleSquare(juce::Colour initialColour, juce::Colour toggleColour, const juce::Image &image)
        : juce::TextButton(""), initialColour(initialColour), toggleColour(toggleColour), buttonImage(image) {
        setOpaque(true);
        setColour(juce::TextButton::buttonColourId, initialColour);
        setClickingTogglesState(true);
    }

    void paintButton(juce::Graphics &g, bool isMouseOverButton, bool isButtonDown) override {
        g.fillAll(findColour(juce::TextButton::buttonColourId));
        if (!buttonImage.isNull()) {
            g.drawImageWithin(buttonImage, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::stretchToFit, 1.0f);
        }
        isMouseOverButton ? g.setColour(juce::Colours::white) : g.setColour(juce::Colours::darkgrey);
        g.drawRect(getLocalBounds(), 2);
    }

    void clicked() override {
        setColour(juce::TextButton::buttonColourId, getToggleState() ? toggleColour : initialColour);
        repaint();
    }

private:
    juce::Colour initialColour;
    juce::Colour toggleColour;
    juce::Image buttonImage;
};

class DownPanel : public juce::Component {
public:
    static constexpr int rows = 8;
    static constexpr int cols = 8;
    static constexpr int padding = 8;

    DownPanel() {
        containerFlex.flexDirection = juce::FlexBox::Direction::row;
        containerFlex.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        containerFlex.alignItems = juce::FlexBox::AlignItems::stretch;

        columnFlexes.resize(cols); // Initialize the vector

        for (int i = 0; i < cols; ++i) {
            auto columnButton = std::make_unique<ToggleSquare>(juce::Colours::grey, juce::Colours::blue, juce::Image());
            auto slider = std::make_unique<juce::Slider>();

            slider->setSliderStyle(juce::Slider::LinearVertical);
            slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
            columnButtons.add(std::move(columnButton));
            sliders.add(std::move(slider));
            addAndMakeVisible(columnButtons[i]);
            addAndMakeVisible(sliders[i]);

            columnFlexes[i].flexDirection = juce::FlexBox::Direction::column;
            columnFlexes[i].justifyContent = juce::FlexBox::JustifyContent::center;
            columnFlexes[i].alignItems = juce::FlexBox::AlignItems::center;

            columnFlexes[i].items.add(juce::FlexItem(*columnButtons[i]).withFlex(1).withWidth(20));
            columnFlexes[i].items.add(juce::FlexItem(*sliders[i]).withFlex(3).withWidth(20));
            //columnButtons[i]->setSize(30, 30);
            //sliders[i]->setSize(20, 100);

            containerFlex.items.ensureStorageAllocated(cols);
            containerFlex.items.add(juce::FlexItem(columnFlexes[i]).withFlex(1).withWidth(20));
        }
        DBG("DownPanel Constructed");
        DBG("Column Buttons size: " + juce::String(columnButtons.size()));
        DBG("Sliders size: " + juce::String(sliders.size()));
    }

    void resized() override {
        DBG("DownPanel Resized");
        containerFlex.performLayout(getLocalBounds().toFloat());
    }

private:
    juce::OwnedArray<ToggleSquare> columnButtons;
    juce::OwnedArray<juce::Slider> sliders;
    juce::FlexBox containerFlex;
    std::vector<juce::FlexBox> columnFlexes; // Use a vector of FlexBox
};

class apcStepperGrid : public juce::AudioProcessorEditor {
public:
    static constexpr int rows = 8;
    static constexpr int cols = 8;
    static constexpr int padding = 2;
    // Constructor: initializes the editor and attaches UI components to processor parameters.
    apcStepperGrid(apcStepperMainProcessor &p)
        : AudioProcessorEditor(&p), processor(p) {
        juce::Array<juce::Colour> rowColours = {
            juce::Colours::red, juce::Colours::orange, juce::Colours::yellow, juce::Colours::green,
            juce::Colours::blue, juce::Colours::indigo, juce::Colours::violet, juce::Colours::pink
        };
        auto* imageData = BinaryData::shadow_png;
        auto imageSize = BinaryData::shadow_pngSize;
        // Load image from Assets folder
        // juce::File imageFile = juce::File::getSpecialLocation(juce::File::currentApplicationFile)
        //         .getParentDirectory()
        //         .getChildFile("Assets/shadow.png");
        //
        // DBG("Looking for image at: " + imageFile.getFullPathName());
        //
        // if (!imageFile.existsAsFile()) {
        //     DBG("ERROR: Image file not found!");
        // }
        //
        // juce::Image buttonImage;
        // if (imageFile.existsAsFile()) {
        //     buttonImage = juce::ImageFileFormat::loadFrom(imageFile);
        // }

        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                auto square = std::make_unique<ToggleSquare>(juce::Colours::lightgrey,
                                                             juce::Colour(rowColours[row % rowColours.size()]),
                                                             Image());


                addAndMakeVisible(*square);
                squares.add(std::move(square));
            }
        }
    }

    void resized() override {
        float cellWidth = static_cast<float>(getWidth()) / cols;
        float cellHeight = static_cast<float>(getHeight()) / rows;
        float squareSize = juce::jmin(cellWidth, cellHeight) - 2 * padding;

        juce::FlexBox flexBox;
        flexBox.flexDirection = juce::FlexBox::Direction::column;
        flexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        flexBox.alignItems = juce::FlexBox::AlignItems::stretch;

        for (int row = 0; row < rows; ++row) {
            juce::FlexBox rowBox;
            rowBox.flexDirection = juce::FlexBox::Direction::row;
            rowBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
            rowBox.alignItems = juce::FlexBox::AlignItems::stretch;
            rowBox.items.ensureStorageAllocated(cols);

            for (int col = 0; col < cols; ++col) {
                int index = row * cols + col;
                squares[index]->setBounds(
                    col * cellWidth + padding,
                    row * cellHeight + padding,
                    squareSize,
                    squareSize);
                rowBox.items.add(juce::FlexItem(*squares[index]).withWidth(squareSize).withHeight(squareSize));
            }

            flexBox.items.add(juce::FlexItem(rowBox).withWidth(getWidth()).withHeight(cellHeight));
        }

        flexBox.performLayout(getLocalBounds().toFloat());
    }

private:
    apcStepperMainProcessor &processor;
    juce::OwnedArray<ToggleSquare> squares;
};

class apcControlPanel : public juce::AudioProcessorEditor {
public:
    static constexpr int rows = 8;
    static constexpr int cols = 8;

    apcControlPanel(apcStepperMainProcessor &p)
        : AudioProcessorEditor(&p), processor(p), grid(p) {
        addAndMakeVisible(grid);
        addAndMakeVisible(emptySpace);

        for (int i = 0; i < rows; ++i) {
            auto rowButton = std::make_unique<ToggleSquare>(juce::Colours::grey, juce::Colours::blue, juce::Image());
            rowButtons.add(std::move(rowButton));
        }
        for (auto *btn: rowButtons)
            addAndMakeVisible(btn);
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
    juce::FlexBox containerDownFlex;
    juce::FlexBox gridFlexBox;

    // Set up main layout properties
    mainFlexBox.flexDirection = juce::FlexBox::Direction::row;
    mainFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    mainFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;


        containerFlex.flexDirection = juce::FlexBox::Direction::column;
        containerFlex.justifyContent = juce::FlexBox::JustifyContent::flexStart;  // Ensure full width
        containerFlex.alignItems = juce::FlexBox::AlignItems::stretch;
    containerDownFlex.flexDirection = juce::FlexBox::Direction::row;
    containerDownFlex.justifyContent = juce::FlexBox::JustifyContent::center;  // Ensure full width
    containerDownFlex.alignItems = juce::FlexBox::AlignItems::stretch;

    juce::FlexBox rightPanel;
    rightPanel.flexDirection = juce::FlexBox::Direction::column;
    rightPanel.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;

    // Add row buttons
    for (int i = 0; i < rows; ++i) {
        rightPanel.items.add(juce::FlexItem(*rowButtons[i]).withFlex(1).withMargin(juce::FlexItem::Margin(4)));
    }

    // Grid FlexBox setup
    gridFlexBox.flexDirection = juce::FlexBox::Direction::column;
    gridFlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
    gridFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;

    // Add grid and rightPanel to mainFlexBox
    mainFlexBox.items.add(juce::FlexItem(grid).withFlex(4));  // Grid takes up most space
    mainFlexBox.items.add(juce::FlexItem(rightPanel).withFlex(1));  // Right panel takes less space
    containerDownFlex.items.add(juce::FlexItem(downPanel).withFlex(4));  // Right panel takes less space
    containerDownFlex.items.add(juce::FlexItem(emptySpace).withFlex(1));  // Right panel takes less space

    // Add everything to containerFlex
    containerFlex.items.add(juce::FlexItem(mainFlexBox).withFlex(3));
    containerFlex.items.add(juce::FlexItem(containerDownFlex).withFlex(1));
    // Perform Layouts
    containerFlex.performLayout(getLocalBounds().toFloat());// Ensure full height for DownPanel
}

private:
    apcStepperMainProcessor &processor;
    apcStepperGrid grid;
    juce::OwnedArray<ToggleSquare> rowButtons;

    DownPanel downPanel;
    juce::Component emptySpace;
};
