#pragma once

#include "ToggleSquare.h"

class ShiftToggleSquareButton : public ToggleSquare {
public:
    ShiftToggleSquareButton(juce::Colour initialColour, juce::Colour toggleColour,
                             juce::Colour shiftToggleColour, const juce::Image& image)
        : ToggleSquare(initialColour, toggleColour, image),
          shiftToggleColour(shiftToggleColour) {}

    void setShiftMode(bool shiftMode) {
        this->shiftMode = shiftMode;
        updateColour();
    }

    void clicked() override {
        ToggleSquare::clicked();
        updateColour();
    }

private:
    juce::Colour shiftToggleColour;
    bool shiftMode = false;

    void updateColour() {
        if (shiftMode) {
            setColour(juce::TextButton::buttonColourId, getToggleState() ? shiftToggleColour : getInitialColour());
        } else {
            setColour(juce::TextButton::buttonColourId, getToggleState() ? getToggleColour() : getInitialColour());
        }
        repaint();
    }

    juce::Colour getInitialColour() const { return initialColour; }
    juce::Colour getToggleColour() const { return toggleColour; }
};