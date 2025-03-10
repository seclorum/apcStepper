#pragma once

#include "ToggleButton.h"
#include "ToggleIconButton.h"
#include "ShiftToggleButton.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

class apcRightPanel : public juce::Component {
public:
    apcRightPanel();
    void resized() override;

private:
    static constexpr int rows = 8;
    juce::OwnedArray<ShiftToggleButton> rowButtons;
    std::unique_ptr<ToggleButton> playToggleButton;
    std::unique_ptr<ToggleButton> stopToggleButton;
    std::unique_ptr<ToggleButton> shiftToggleButton;
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
