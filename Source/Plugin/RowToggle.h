#pragma once

#include "apcToggleButton.h"
#include <juce_gui_extra/juce_gui_extra.h>

class RowToggle : public juce::TextButton {
public:
    juce::Colour initialColour;
    juce::Colour toggleColour;

    RowToggle(juce::Colour initialColour, juce::Colour toggleColour)
        : initialColour(initialColour), toggleColour(toggleColour),
          isToggled(false) {
        setClickingTogglesState(true);
        setColour(juce::TextButton::buttonColourId, initialColour);
    }

    void clicked() override {
        isToggled = !isToggled;
        setButtonText(getToggleState() ? "Length" : "Velocity");
    }

    void resized() override {
       repaint();
    }

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {


        // Draw the button text centered and scaled
        g.setColour(findColour(juce::TextButton::textColourOnId));
        g.setFont(juce::Font(getHeight() * 0.7f, juce::Font::bold)); // Scale font size

        juce::Rectangle<float> textBounds = getLocalBounds().toFloat();
        g.drawFittedText(getButtonText(), getLocalBounds(), juce::Justification::centred, 1); // Draw centered and scale if needed
        g.fillAll(isToggled ? toggleColour : initialColour);
    }

private:
    const char *iconImage;
    size_t iconImageSize;

    bool isToggled;
};