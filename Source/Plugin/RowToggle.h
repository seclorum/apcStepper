#pragma once

#include "ToggleSquare.h"
#include <juce_gui_extra/juce_gui_extra.h>

class RowToggle : public ToggleSquare {
public:
    RowToggle(juce::Colour initialColour, juce::Colour toggleColour, const juce::Image& image)
        : ToggleSquare(initialColour, toggleColour, image) {
        setClickingTogglesState(true);
        setButtonText("Velocity");
        setSize(200, 20);
        setColour(juce::TextButton::textColourOnId, juce::Colours::black);
        changeWidthToFitText(getHeight());
    }

    void clicked() override {
        ToggleSquare::clicked();
        setButtonText(getToggleState() ? "Length" : "Velocity");
    }

    void resized() override {
        ToggleSquare::resized();
    }

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        ToggleSquare::paintButton(g, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

        // Draw the button text centered and scaled
        g.setColour(findColour(juce::TextButton::textColourOnId));
        g.setFont(juce::Font(getHeight() * 0.7f, juce::Font::bold)); // Scale font size

        juce::Rectangle<float> textBounds = getLocalBounds().toFloat();
        g.drawFittedText(getButtonText(), getLocalBounds(), juce::Justification::centred, 1); // Draw centered and scale if needed
    }

private:
};