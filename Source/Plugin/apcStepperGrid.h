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


#define APCSTEPPER_APCSTEPPERGRID_H
#define APCSTEPPER_APCSTEPPERGRID_H

class apcStepperMainProcessor;

class ToggleSquare : public juce::TextButton
{
public:
    ToggleSquare(juce::Colour initialColour, juce::Colour toggleColour, const juce::Image& image)
        : juce::TextButton(""), initialColour(initialColour), toggleColour(toggleColour), buttonImage(image)
    {
        setOpaque(true);
        setColour(juce::TextButton::buttonColourId, initialColour);
        setClickingTogglesState(true);
    }

    void paintButton(juce::Graphics& g, bool isMouseOverButton, bool isButtonDown) override
    {
        g.fillAll(findColour(juce::TextButton::buttonColourId));
        if (!buttonImage.isNull())
        {
            g.drawImageWithin(buttonImage, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::stretchToFit, 1.0f);

        }
        isMouseOverButton ? g.setColour(juce::Colours::white) : g.setColour(juce::Colours::darkgrey);
        g.drawRect(getLocalBounds(), 2);
    }

    void clicked() override
    {
        setColour(juce::TextButton::buttonColourId, getToggleState() ? toggleColour : initialColour);
        repaint();
    }

private:
    juce::Colour initialColour;
    juce::Colour toggleColour;
    juce::Image buttonImage;
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

        // Load image from Assets folder
        juce::File imageFile = juce::File::getSpecialLocation(juce::File::currentApplicationFile)
                                     .getParentDirectory()
                                     .getChildFile("Assets/shadow.png");

        DBG("Looking for image at: " + imageFile.getFullPathName());

        if (!imageFile.existsAsFile())
        {
            DBG("ERROR: Image file not found!");
        }

        juce::Image buttonImage;
        if (imageFile.existsAsFile())
        {
            buttonImage = juce::ImageFileFormat::loadFrom(imageFile);
        }

        for (int row = 0; row < rows; ++row)
        {
            for (int col = 0; col < cols; ++col)
            {
                auto square = std::make_unique<ToggleSquare>(juce::Colours::lightgrey, juce::Colour(rowColours[row % rowColours.size()]),buttonImage);


                addAndMakeVisible(*square);
                squares.add(std::move(square));
            }
        }
    }

    void resized() override
    {
        float cellWidth = static_cast<float>(getWidth()) / cols;
        float cellHeight = static_cast<float>(getHeight()) / rows;
        float squareSize = juce::jmin(cellWidth, cellHeight) - 2 * padding;

        juce::FlexBox flexBox;
        flexBox.flexDirection = juce::FlexBox::Direction::column;
        flexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        flexBox.alignItems = juce::FlexBox::AlignItems::stretch;

        for (int row = 0; row < rows; ++row)
        {
            juce::FlexBox rowBox;
            rowBox.flexDirection = juce::FlexBox::Direction::row;
            rowBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
            rowBox.alignItems = juce::FlexBox::AlignItems::stretch;
            rowBox.items.ensureStorageAllocated(cols);

            for (int col = 0; col < cols; ++col)
            {
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

class apcControlPanel : public juce::AudioProcessorEditor
{
public:
    static constexpr int rows = 8;
    static constexpr int cols = 8;

    apcControlPanel(apcStepperMainProcessor &p)
        : AudioProcessorEditor(&p), processor(p), grid(p)
    {
        addAndMakeVisible(grid);
        addAndMakeVisible(emptySpace);

        for (int i = 0; i < rows; ++i)
        {
            auto rowButton = std::make_unique<ToggleSquare>(juce::Colours::grey, juce::Colours::blue, juce::Image());
            auto columnButton = std::make_unique<ToggleSquare>(juce::Colours::grey, juce::Colours::red, juce::Image());

            auto slider = std::make_unique<juce::Slider>();

            slider->setSliderStyle(juce::Slider::LinearVertical);
            slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
            sliders.add(std::move(slider));
            rowButtons.add(std::move(rowButton));
            columnButtons.add(std::move(columnButton));
        }

        for (auto *btn : rowButtons)
            addAndMakeVisible(btn);
        for (auto *btn : columnButtons)
            addAndMakeVisible(btn);
        for (auto *slider : sliders)
            addAndMakeVisible(slider);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        juce::FlexBox mainFlexBox;
        juce::FlexBox containerFlex;
        juce::FlexBox containerDownFlex;

        mainFlexBox.flexDirection = juce::FlexBox::Direction::row;
        mainFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        mainFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;

        containerFlex.flexDirection = juce::FlexBox::Direction::column;
        containerFlex.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        containerFlex.alignItems = juce::FlexBox::AlignItems::stretch;

        containerDownFlex.flexDirection = juce::FlexBox::Direction::row;
        containerDownFlex.justifyContent = juce::FlexBox::JustifyContent::flexStart; // Changed to flexStart
        containerDownFlex.alignItems = juce::FlexBox::AlignItems::stretch;

        juce::FlexBox rightPanel;
        rightPanel.flexDirection = juce::FlexBox::Direction::column;
        rightPanel.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;

        for (int i = 0; i < rows; ++i)
        {
            rightPanel.items.add(juce::FlexItem(*rowButtons[i]).withFlex(1).withMargin(juce::FlexItem::Margin(4)));
        }

        for (int i = 0; i < cols; ++i) {
            juce::FlexBox downPanel;
            downPanel.flexDirection = juce::FlexBox::Direction::row;
            downPanel.justifyContent = juce::FlexBox::JustifyContent::flexStart;
            downPanel.alignItems = juce::FlexBox::AlignItems::stretch;
            downPanel.items.add(juce::FlexItem(*columnButtons[i]).withMaxHeight(true).withMaxWidth(true));
            downPanel.items.add(juce::FlexItem(*sliders[i]).withFlex(3).withMaxHeight(true).withMaxWidth(true));

            containerDownFlex.items.add(juce::FlexItem(downPanel).withFlex(1)); //Added margin

        }

        //containerDownFlex.items.add(juce::FlexItem(emptySpace).withFlex(1));
        mainFlexBox.items.add(juce::FlexItem(grid).withFlex(4));
        mainFlexBox.items.add(juce::FlexItem(rightPanel).withFlex(1));

        containerFlex.items.add(juce::FlexItem(mainFlexBox).withFlex(4));
        containerFlex.items.add(juce::FlexItem(containerDownFlex).withFlex(1).withMinHeight(100));
        //containerDownFlex.setBorder(juce::BorderSize<int>(1)); //debug

        containerFlex.performLayout(bounds.toFloat());
    }
private:
    apcStepperMainProcessor &processor;
    apcStepperGrid grid;
    juce::OwnedArray<ToggleSquare> rowButtons;
    juce::OwnedArray<ToggleSquare> columnButtons;
    juce::OwnedArray<juce::Slider> sliders;
    juce::Component emptySpace;
};
