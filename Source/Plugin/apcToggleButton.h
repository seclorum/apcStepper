#pragma once

#include <BinaryData.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

class apcToggleButton : public juce::TextButton {
public:
    juce::Colour &toggleColour;
    bool isToggled;
    apcToggleButton(juce::Colour initialColour, juce::Colour toggleColour)
        : juce::TextButton(""),initialColour(initialColour), toggleColour(toggleColour), isToggled(false) {

        setClickingTogglesState(true); // Enables automatic toggling
        setColour(juce::TextButton::buttonColourId, initialColour);
    }

    void buttonStateChanged() override {
        isToggled = getToggleState(); // Sync state
        repaint();
    }
    void setColourToggle(juce::Colour newColour) {
        // Store in JUCE's colour system
        toggleColour = newColour;// Retrieve updated colour
        repaint(); // Ensure UI updates
    }
    void setToggleState(bool newState, bool shouldAnimate = true)  {
        juce::TextButton::setToggleState(newState, shouldAnimate);
        isToggled = newState;
        repaint();
    }

    bool getToggleState() const  {
        return juce::TextButton::getToggleState();
    }

    void paintButton(juce::Graphics &g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {
        g.fillAll(isToggled ? toggleColour : initialColour);

        if (auto drawable = juce::Drawable::createFromImageData(BinaryData::button_svg, BinaryData::button_svgSize)) {
            drawable->drawWithin(g, drawable->getDrawableBounds(), juce::RectanglePlacement::fillDestination, 1.0f);
        }
    }

private:
    juce::Colour initialColour;

};
