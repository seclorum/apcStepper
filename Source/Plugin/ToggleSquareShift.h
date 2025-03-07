#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "ToggleSquare.h"

class ToggleSquareShift : public juce::Component {
public:
    ToggleSquareShift(juce::OwnedArray<ToggleSquare>& rowButtons,
                        std::unique_ptr<ToggleSquare>& playToggleButton,
                        std::unique_ptr<ToggleSquare>& stopToggleButton,
                        std::unique_ptr<ToggleSquare>& shiftToggleButton);

    void resized() override;

private:
    juce::OwnedArray<ToggleSquare>& rowButtons;
    std::unique_ptr<ToggleSquare>& playToggleButton;
    std::unique_ptr<ToggleSquare>& stopToggleButton;
    std::unique_ptr<ToggleSquare>& shiftToggleButton;

    juce::FlexBox rightPanel;
    juce::FlexBox downButtons;
    juce::Component downButtonsBoundsComponent; // Dummy component
    bool shiftMode = false;
    void updateRowButtonColors();
};