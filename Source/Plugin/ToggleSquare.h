// Created by Tom Peak Walcher on 05.03.25.
//

#ifndef TOGGLESQUARE_H
#define TOGGLESQUARE_H

#include <juce_gui_basics/juce_gui_basics.h>

class ToggleSquare : public juce::TextButton {
public:
    juce::Colour initialColour;
    juce::Colour toggleColour;
    ToggleSquare(juce::Colour initialColour, juce::Colour toggleColour, const juce::Image &image)
        : juce::TextButton(""), initialColour(initialColour), toggleColour(toggleColour), buttonImage(image) {
        setOpaque(true);
        setColour(juce::TextButton::buttonColourId, initialColour);
        setClickingTogglesState(true);
    }

    void paintButton(juce::Graphics &g, bool isMouseOverButton, bool isButtonDown) override {
        g.fillAll(findColour(juce::TextButton::buttonColourId));
        if (!buttonImage.isNull()) {
            g.drawImageWithin(buttonImage, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::fillDestination, 1.0f);
        }
        if (transformOriginCentre) {
            g.saveState();
            g.addTransform(juce::AffineTransform::translation(-getWidth() / 2.0f, -getHeight() / 2.0f));
            g.reduceClipRegion(getLocalBounds());
            g.restoreState();
        }
        isMouseOverButton ? g.setColour(juce::Colours::white) : g.setColour(juce::Colours::darkgrey);
        g.drawRect(getLocalBounds(), 2);
    }

    void setTransformOriginToCentre(bool shouldCenter) {
        transformOriginCentre = shouldCenter;
        repaint();
    }

    void clicked() override {
        setColour(juce::TextButton::buttonColourId, getToggleState() ? toggleColour : initialColour);
        repaint();
    }

private:

    juce::Image buttonImage;
    bool transformOriginCentre = true;
};

#endif // TOGGLESQUARE_H