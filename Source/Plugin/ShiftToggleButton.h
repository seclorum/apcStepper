#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

class ShiftToggleButton : public apcToggleButton {
public:

    ShiftToggleButton(juce::Colour initialColour, juce::Colour toggleColour,juce::Colour toggleShiftColour,bool isShift)
    : apcToggleButton(initialColour, toggleColour), toggleShiftColour(toggleShiftColour), isShift(isShift),isToggled(false) {
        setClickingTogglesState(true);
        setColour(juce::TextButton::buttonColourId, initialColour);
    }

    void clicked() override {
        apcToggleButton::clicked();

    }
    void setToggleState(bool newState, bool shouldAnimate = true)  {
        juce::TextButton::setToggleState(newState, shouldAnimate);
        isToggled = newState;
        repaint();
    }

    bool getToggleState() const  {
        return juce::TextButton::getToggleState();
    }
    void setShift(bool mode) {

        isShift = mode;
        setColourToggle( isShift ? toggleShiftColour : toggleColour);
        repaint();
    }

private:
    juce::Colour initialColour;
    juce::Colour toggleColour;
    juce::Colour toggleShiftColour;
    bool isShift;
    bool isToggled;
};