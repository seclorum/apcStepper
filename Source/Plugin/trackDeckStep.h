// Created by Jay Vaughan on 26.02.25.
//

#include "Common.h"

#include "RowToggle.h"
#include "apcLog.h"
#include "apcToggleButton.h"
#include "apcToggleParameterButton.h"

class trackDeckMainProcessor;

class trackDeckStep : public juce::AudioProcessorEditor, private juce::Timer {
public:
    static constexpr int rows = 8;
    static constexpr int padding = 2;

    trackDeckStep(trackDeckMainProcessor &p, const int s)
        : AudioProcessorEditor(&p), processor(p), stepNumber(s) {
        juce::Colour red900 = juce::Colour(0xffB71C1C); juce::Colour pink900 = juce::Colour(0xff880E4F); juce::Colour purple900 = juce::Colour(0xff4A148C); juce::Colour deepPurple900 = juce::Colour(0xff311B92); juce::Colour indigo900 = juce::Colour(0xff1A237E); juce::Colour blue900 = juce::Colour(0xff0D47A1); juce::Colour lightBlue900 = juce::Colour(0xff01579B); juce::Colour cyan900 = juce::Colour(0xff006064); juce::Colour teal900 = juce::Colour(0xff00695C); juce::Colour green900 = juce::Colour(0xff1B5E20); juce::Colour lightGreen900 = juce::Colour(0xff33691E); juce::Colour lime900 = juce::Colour(0xff827717); juce::Colour yellow900 = juce::Colour(0xffF57F17); juce::Colour amber900 = juce::Colour(0xffFF6F00); juce::Colour orange900 = juce::Colour(0xffE65100); juce::Colour deepOrange900 = juce::Colour(0xffBF360C); juce::Colour brown900 = juce::Colour(0xff3E2723); juce::Colour grey900 = juce::Colour(0xff212121); juce::Colour blueGrey900 = juce::Colour(0xff263238);

        juce::Array<juce::Colour> rowColours = {
           red900,pink900,purple900,orange900,lightBlue900,indigo900,teal900,deepPurple900,deepOrange900
        };


        // Initialize row squares (1 column of ToggleSquares)

        for (int row = 0; row < rows; row++) {
            auto square = std::make_unique<apcToggleParameterButton>(
                "s" + processor.addLeadingZeros(stepNumber) + "t" + processor.addLeadingZeros(row),
                stepNumber, row, Colours::aliceblue, rowColours[row], processor);

            //APCLOG("step_" + std::to_string(stepNumber) + "_track_" + std::to_string(row));
            addAndMakeVisible(*square);
            squares.add(std::move(square));

            APCLOG("step_" + std::to_string(stepNumber) + "_track_" + std::to_string(row));
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
        columnFlexBox.items.add(juce::FlexItem(rowFlexBox).withFlex(4));
        columnFlexBox.items.add(juce::FlexItem().withMargin(8));

        // Add controlFlexBox (row toggle & slider) with flex 2
        columnFlexBox.items.add(juce::FlexItem(controlFlexBox).withFlex(2));

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
