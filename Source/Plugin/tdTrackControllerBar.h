// Created by Jay Vaughan on 26.02.25.
//

#include "Common.h"

#include "RowToggle.h"
#include "apcLog.h"
#include "tdTrackController.h"

class trackDeckMainProcessor;

class tdTrackControllerBar : public juce::AudioProcessorEditor {
public:

    static constexpr int padding = 6.0f;

    tdTrackControllerBar(trackDeckMainProcessor &p, const int s)
        : AudioProcessorEditor(&p), processor(p), stepNumber(s) {
        static constexpr int steps = 16;
        // Initialize row squares (1 column of ToggleSquares)

        for (int col = 0; col < steps; col++) {
            auto square = std::make_unique<tdTrackController>(processor,col);

            //APCLOG("step_" + std::to_string(stepNumber) + "_track_" + std::to_string(row));
            addAndMakeVisible(*square);
            squares.add(std::move(square));

            APCLOG("step_" + std::to_string(stepNumber) + "_track_" + std::to_string(col));
        }


    }

    void resized() override {
        auto bounds = getLocalBounds();
        float squareSize = bounds.getWidth() - padding; // Fit squares + row toggle

        // Create a FlexBox for the squares (step grid)
        juce::FlexBox rowFlexBox;
        rowFlexBox.flexDirection = juce::FlexBox::Direction::row;
        rowFlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
        rowFlexBox.alignItems = juce::FlexBox::AlignItems::center;
        juce::FlexBox rowFlexContainer;
        rowFlexContainer.flexDirection = juce::FlexBox::Direction::row;
        rowFlexContainer.justifyContent = juce::FlexBox::JustifyContent::center;
        rowFlexContainer.alignItems = juce::FlexBox::AlignItems::center;

        // Add squares to rowFlexBox
        for (auto &square: squares) {
            rowFlexBox.items.add(
                juce::FlexItem(*square).withWidth(squareSize).withHeight(squareSize).withFlex(1).withMargin(6));
        }

        rowFlexContainer.items.add(
                        juce::FlexItem(rowFlexBox).withFlex(12));
        rowFlexContainer.items.add(
                 juce::FlexItem().withFlex(1));


        rowFlexContainer.performLayout(bounds.toFloat());
    }


private:
    trackDeckMainProcessor &processor;
    juce::OwnedArray<tdTrackController> squares;



    const int stepNumber;
};
