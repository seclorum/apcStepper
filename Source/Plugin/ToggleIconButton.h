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
        apcToggleButton::clicked();
    }



    void paintOverChildren(juce::Graphics &g) override {
        auto iconImageArea = getBoundsInParent().toFloat();
        if (iconImage && iconImageSize) {
            auto drawAble = juce::Drawable::createFromImageData(iconImage, iconImageSize);
            drawAble->drawWithin(g, iconImageArea, juce::RectanglePlacement::xRight, 1.f);
        }
        //		g.drawImage(iconImage, getLocalBounds().toFloat());
    }


private:
    const char *iconImage;
    size_t iconImageSize;

};
