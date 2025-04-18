// Created by Jay Vaughan on 26.02.25.
//

#include "Common.h"

#include "RowToggle.h"
#include "apcLog.h"
#include "apcToggleButton.h"
#include "apcToggleParameterButton.h"

class trackDeckMainProcessor;

class tdTrackController : public juce::AudioProcessorEditor, private juce::Timer {
public:

    static constexpr int padding = 6.0f;

    tdTrackController(trackDeckMainProcessor &p, const int s)
        : AudioProcessorEditor(&p), processor(p), stepNumber(s) {

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
        /*
                fatButton_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                        processor.getParameters(), "fatButton_" + std::to_string(stepNumber), *rowToggle);

                slider_attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                        processor.getParameters(), "slider_" + std::to_string(stepNumber), *slider);
        */
        addAndMakeVisible(*slider); // Default size
        APCLOG("trackDeckTrack initialized...");
        startTimer(25);
    }

    void resized() override {
        auto bounds = getLocalBounds();
        float squareSize = bounds.getWidth() - padding; // Fit squares + row toggle

        // Create a FlexBox for the squares (step grid)

        // Create a FlexBox for rowToggle and slider
        juce::FlexBox controlFlexBox;
        controlFlexBox.flexDirection = juce::FlexBox::Direction::column;
        controlFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        controlFlexBox.alignItems = juce::FlexBox::AlignItems::center;

        // Add row toggle button
        controlFlexBox.items.add(
            juce::FlexItem(*rowToggle).withFlex(1).withWidth(bounds.getWidth() - 12).withMargin(8));

        // Add vertical slider
        controlFlexBox.items.add(
            juce::FlexItem(*slider).withFlex(5).withWidth(getWidth()).withMargin(8));

        // Create main column FlexBox
        juce::FlexBox columnFlexBox;
        columnFlexBox.flexDirection = juce::FlexBox::Direction::column;
        columnFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        columnFlexBox.alignItems = juce::FlexBox::AlignItems::center;

        // Add rowFlexBox (squares) with flex 4


        // Add controlFlexBox (row toggle & slider) with flex 2
        columnFlexBox.items.add(juce::FlexItem(controlFlexBox).withFlex(1));

        // Apply layout
        columnFlexBox.performLayout(bounds.toFloat());
    }

    void timerCallback() override {
        for (int i = 0; i < processor.numSteps; ++i) {
            std::string parameterID = "c" + processor.addLeadingZeros(i);


            if (auto *param = dynamic_cast<AudioParameterBool *>(processor.parameters.getParameter(parameterID))) {
                if (i == processor.currentMIDIStep) param->setValueNotifyingHost(1.0f); // true -> 1.0f, false -> 0.0f
                else param->setValueNotifyingHost(0.0f); // true -> 1.0f, false -> 0.0f
            } else {
                DBG("Parameter " + parameterID + " not found or not a bool!");
            }
        }
    }

    void setColumnforStep(int step) {
        if (step == stepNumber) {
            setColour(Label::backgroundColourId, Colours::orange);
        } else {
            setColour(Label::backgroundColourId, Colours::white);
        }
    }

private:
    trackDeckMainProcessor &processor;
    juce::OwnedArray<apcToggleParameterButton> squares;
    juce::Image shadowImage;
    std::unique_ptr<juce::Slider> slider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> slider_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> fatButton_attachment;

    std::unique_ptr<RowToggle> rowToggle;

    const int stepNumber;
};
