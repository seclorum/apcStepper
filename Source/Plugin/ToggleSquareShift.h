#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "ToggleSquare.h"

class ToggleSquareShift : public juce::Component {
public:
    ToggleSquareShift();
    int rows = 8;

    void resized() override;

private:
    juce::OwnedArray<ToggleSquare> rowButtons;
    std::unique_ptr<ToggleSquare> playToggleButton;
    std::unique_ptr<ToggleSquare> stopToggleButton;
    std::unique_ptr<ToggleSquare> shiftToggleButton;
    juce::Image playButton;
    juce::Image stopButton;
    juce::Image shiftButton;
    juce::FlexBox rightPanel;
    juce::FlexBox downButtons;
    juce::FlexBox rightPanelContainer;
    juce::Component downButtonsBoundsComponent; // Dummy component
    bool shiftMode = false;
    void updateRowButtonColors();
};