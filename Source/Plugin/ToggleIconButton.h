// ToggleSquare.h
#pragma once

#include "apcToggleButton.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

class ToggleIconButton : public apcToggleButton {
public:
    juce::Colour initialColour;
    juce::Colour toggleColour;

    ToggleIconButton(juce::Colour initialColour, juce::Colour toggleColour, const char *image, size_t imagesize)
      : apcToggleButton(initialColour, toggleColour), iconImage(image), iconImageSize(imagesize) {}

    void clicked() override {
        isToggled = !isToggled;
        repaint();
        if (onClick)
            onClick();
    }

    void setToggleState(bool newState, bool shouldAnimate = true) {
        isToggled = newState;
        repaint();
    }

    bool getToggleState() const {
        return isToggled;
    }

    void paintButton(juce::Graphics &g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {
        auto iconImageArea = juce::Rectangle<float>(0, 0, getWidth(), getHeight());
        if (iconImage && iconImageSize) {
            auto drawAble = juce::Drawable::createFromImageData(iconImage, iconImageSize);
            drawAble->drawWithin(g, iconImageArea, juce::RectanglePlacement::xRight, 1.f);
        }
        //		g.drawImage(iconImage, getLocalBounds().toFloat());
        g.fillAll(isToggled ? toggleColour : initialColour);
    }

    std::function<void()> onClick;

private:
    const char *iconImage;
    size_t iconImageSize;

    bool isToggled;
};
