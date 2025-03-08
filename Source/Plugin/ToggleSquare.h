// ToggleSquare.h
#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class ToggleSquare : public juce::TextButton {
public:
    juce::Colour initialColour;
    juce::Colour toggleColour;
    ToggleSquare(juce::Colour initialColour, juce::Colour toggleColour, const juce::Image& image)
        : initialColour(initialColour), toggleColour(toggleColour), shadowImage(image), isToggled(false) {
        setClickingTogglesState(true);
        setColour(juce::TextButton::buttonColourId, initialColour);
    }

    void clicked() override {
        isToggled = !isToggled;
        repaint();
        if (onClick)
            onClick();
    }

    void setToggleState(bool newState, bool shouldAnimate=true) {
        isToggled = newState;
        repaint();
    }

    bool getToggleState() const {
        return isToggled;
    }

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {
        g.drawImage(shadowImage, getLocalBounds().toFloat());
        g.fillAll(isToggled ? toggleColour : initialColour);
    }

    std::function<void()> onClick;

private:

    juce::Image shadowImage;
    bool isToggled;
};