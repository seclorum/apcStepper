// Created by Jay Vaughan on 26.02.25.
//

#include "Common.h"

#include "RowToggle.h"
#include "apcToggleButton.h"
#include "apcToggleParameterButton.h"

class apcStepperMainProcessor;

class apcStepperStep : public juce::AudioProcessorEditor {
public:
    static constexpr int rows = 8;
    static constexpr int padding = 2;

    apcStepperStep(apcStepperMainProcessor &p, const int t)
        : AudioProcessorEditor(&p), processor(p), trackNumber(t) {
        // Define row colors for each step in the sequence
        juce::Array<juce::Colour> rowColours = {
            juce::Colours::red, juce::Colours::orange, juce::Colours::yellow, juce::Colours::green,
            juce::Colours::blue, juce::Colours::indigo, juce::Colours::violet, juce::Colours::pink
        };


        // Initialize row squares (1 column of ToggleSquares)
        for (int row = 0; row < rows; ++row) {
            auto square = std::make_unique<apcToggleParameterButton>(("step_" + std::to_string(trackNumber) + "_track_" + std::to_string(row)  ),
                juce::Colours::lightgrey, rowColours[row],processor);

            addAndMakeVisible(*square);
            squares.add(std::move(square));
        }

        // Row toggle button
        rowToggle = std::make_unique<RowToggle>(juce::Colours::yellowgreen, Colours::lightblue);
        addAndMakeVisible(rowToggle.get());

        // Add a **VERTICAL** slider
        // Create a slider (default constructor)
        slider = std::make_unique<juce::Slider>();

        // Set the slider to be vertical
        slider->setSliderStyle(juce::Slider::LinearVertical);
        slider->setRange(0.0, 1.0, 0.01);
        slider->setValue(0.5);
        slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0); // Hide text box
        addAndMakeVisible(*slider); // Default size
        APCLOG("apcStepperTrack initialized...");
    }

    void resized() override {
        auto bounds = getLocalBounds();
        float squareSize = bounds.getHeight() / (rows + 2) - 2 * padding; // Fit squares + row toggle

        // Create a FlexBox for the squares (step grid)
        juce::FlexBox rowFlexBox;
        rowFlexBox.flexDirection = juce::FlexBox::Direction::column;
        rowFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        rowFlexBox.alignItems = juce::FlexBox::AlignItems::center;

        // Add squares to rowFlexBox
        for (auto &square: squares) {
            rowFlexBox.items.add(
                juce::FlexItem(*square).withWidth(squareSize).withHeight(squareSize).withFlex(1).withMargin(6));
        }

        // Create a FlexBox for rowToggle and slider
        juce::FlexBox controlFlexBox;
        controlFlexBox.flexDirection = juce::FlexBox::Direction::column;
        controlFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        controlFlexBox.alignItems = juce::FlexBox::AlignItems::center;

        // Add row toggle button
        controlFlexBox.items.add(
            juce::FlexItem(*rowToggle).withFlex(1).withWidth(bounds.getWidth() - 12));

        // Add vertical slider
        controlFlexBox.items.add(
            juce::FlexItem(*slider).withFlex(5).withWidth(getWidth()));

        // Create main column FlexBox
        juce::FlexBox columnFlexBox;
        columnFlexBox.flexDirection = juce::FlexBox::Direction::column;
        columnFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        columnFlexBox.alignItems = juce::FlexBox::AlignItems::center;

        // Add rowFlexBox (squares) with flex 4
        columnFlexBox.items.add(juce::FlexItem(rowFlexBox).withFlex(4));

        // Add controlFlexBox (row toggle & slider) with flex 2
        columnFlexBox.items.add(juce::FlexItem(controlFlexBox).withFlex(2));

        // Apply layout
        columnFlexBox.performLayout(bounds.toFloat());
    }

private:
    apcStepperMainProcessor &processor;
    juce::OwnedArray<apcToggleParameterButton> squares;
    juce::Image shadowImage;
    std::unique_ptr<juce::Slider> slider;
    std::unique_ptr<RowToggle> rowToggle;
    const int trackNumber;
};
