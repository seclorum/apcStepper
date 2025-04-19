// Created by Jay Vaughan on 26.02.25.
//

#include "Common.h"

#include "RowToggle.h"
#include "apcLog.h"
#include "apcToggleButton.h"
#include "apcToggleParameterButton.h"

class trackDeckMainProcessor;

class tdTrack : public juce::AudioProcessorEditor, private juce::Timer {
public:

    static constexpr int padding = 6.0f;

    tdTrack(trackDeckMainProcessor &p, const int i)
        : AudioProcessorEditor(&p), processor(p), instrumentNumber(i){
        static constexpr int steps = 16;
        // Initialize row squares (1 column of ToggleSquares)

        for (int col = 0; col < steps; ++col) {
            auto square = std::make_unique<apcToggleParameterButton>(
                "s" + processor.addLeadingZeros(col) + "t" + processor.addLeadingZeros(instrumentNumber),
                col, instrumentNumber, Colours::aliceblue, processor);

            //APCLOG("step_" + std::to_string(stepNumber) + "_track_" + std::to_string(row));
            addAndMakeVisible(*square);
            squares.add(std::move(square));

            APCLOG("step_" + std::to_string(instrumentNumber) + "_track_" + std::to_string(col));
        }
        muteButton = std::make_unique<apcToggleButton>(Colours::darkgrey, Colours::darkred);

        //APCLOG("step_" + std::to_string(stepNumber) + "_track_" + std::to_string(row));
        addAndMakeVisible(*muteButton);

        startTimer(25);
    }

    void resized() override {
        auto bounds = getLocalBounds();
        float squareSize = bounds.getHeight() - padding; // Fit squares + row toggle

        // Create a FlexBox for the squares (step grid)
        juce::FlexBox rowFlexBox;
        rowFlexBox.flexDirection = juce::FlexBox::Direction::row;
        rowFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        rowFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;

        // Add squares to rowFlexBox
        for (auto &square: squares) {
            rowFlexBox.items.add(
                juce::FlexItem(*square).withFlex(1).withMargin(6));
        }


        // Create main column FlexBox
        juce::FlexBox trackFlexBox;
        trackFlexBox.flexDirection = juce::FlexBox::Direction::row;
        trackFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        trackFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;

        // Add rowFlexBox (squares) with flex 4
        trackFlexBox.items.add(juce::FlexItem(rowFlexBox).withFlex(12));
        trackFlexBox.items.add(juce::FlexItem(*muteButton).withFlex(1));
        // Apply layout
        trackFlexBox.performLayout(bounds.toFloat());
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

private:
    trackDeckMainProcessor &processor;
    juce::OwnedArray<apcToggleParameterButton> squares;
    juce::Image shadowImage;
    std::unique_ptr<apcToggleButton> muteButton;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> slider_attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> fatButton_attachment;

    std::unique_ptr<RowToggle> rowToggle;

    const int instrumentNumber;
};
