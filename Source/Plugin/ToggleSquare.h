//
// Created by Tom Peak Walcher on 05.03.25.
//

#ifndef TOGGLESQUARE_H
#define TOGGLESQUARE_H


class ToggleSquare : public juce::TextButton {
public:
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
        isMouseOverButton ? g.setColour(juce::Colours::white) : g.setColour(juce::Colours::darkgrey);
        g.drawRect(getLocalBounds(), 2);
    }

    void clicked() override {
        setColour(juce::TextButton::buttonColourId, getToggleState() ? toggleColour : initialColour);
        repaint();
    }

private:
    juce::Colour initialColour;
    juce::Colour toggleColour;
    juce::Image buttonImage;
};


#endif //TOGGLESQUARE_H
