// ToggleSquare.h
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class ShiftToggleSquareButton : public juce::TextButton {
public:
    juce::Colour initialColour;
    juce::Colour toggleColour;
    juce::Colour toggleShiftColour;
    ShiftToggleSquareButton(juce::Colour initialColour, juce::Colour toggleColour,juce::Colour toggleShiftColour, const juce::Image& image)
        : initialColour(initialColour), toggleColour(toggleColour), toggleShiftColour(toggleShiftColour), shadowImage(image), isToggled(false) , isShift(false) {
        setClickingTogglesState(true);
        setColour(juce::TextButton::buttonColourId, initialColour);
    }

    void clicked() override {
        isToggled = !isToggled;
        repaint();
    }

    void setToggleState(bool newState, bool shouldAnimate=true) {
        isToggled = newState;
        repaint();
    }
    void setShift(bool newState) {
        isShift = newState;
        repaint();
    }

    bool getToggleState() const {
        return isToggled;
    }

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {
        g.drawImage(shadowImage, getLocalBounds().toFloat());
        g.fillAll(isToggled ? isShift ? toggleShiftColour : toggleColour : initialColour);
    }



private:

    juce::Image shadowImage;
    bool isToggled;
    bool isShift;
};