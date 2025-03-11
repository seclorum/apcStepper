#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

class ShiftToggleButton : public apcToggleButton {
public:

    ShiftToggleButton(juce::Colour initialColour, juce::Colour toggleColour,juce::Colour toggleShiftColour,bool isShift)
    : apcToggleButton(initialColour, toggleColour), toggleColour(toggleColour),toggleShiftColour(toggleShiftColour), isShift(isShift) {
        setClickingTogglesState(true);
        setColour(juce::TextButton::buttonColourId, initialColour);
    }

    void clicked() override {
        apcToggleButton::clicked();

    }

    void setShift(bool mode) {

        isShift = mode;
        setColourToggle(isShift ? toggleShiftColour : toggleColour);
        resized();
    }

private:
    juce::Colour toggleShiftColour;
    juce::Colour toggleColour;
    bool isShift;

};