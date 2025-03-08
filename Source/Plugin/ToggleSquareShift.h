#pragma once

#include "ToggleSquare.h"
#include "ShiftToggleSquareButton.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

class ToggleSquareShift : public juce::Component {
public:
    ToggleSquareShift();
    void resized() override;

private:
    static constexpr int rows = 8;
    juce::OwnedArray<ShiftToggleSquareButton> rowButtons;
    juce::Image playButton;
    juce::Image stopButton;
    juce::Image shiftButton;
    std::unique_ptr<ToggleSquare> playToggleButton;
    std::unique_ptr<ToggleSquare> stopToggleButton;
    std::unique_ptr<ToggleSquare> shiftToggleButton;
    juce::FlexBox rightPanel;
    juce::FlexBox downButtons;
    juce::FlexBox rightPanelContainer;
    bool shiftMode = false;

    void squareClicked(int index) {
        for (int i = 0; i < rows; ++i) {
            if (i != index) {
                rowButtons[i]->setToggleState(false, false);
            }
        }
        rowButtons[index]->setToggleState(true, false);
    }

    void updateRowButtonColors() {
        for (auto& button : rowButtons) {
            button->setShift(shiftMode);
        }
    }
};
