#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

class ShiftToggleSquareButton : public juce::TextButton {
public:
    juce::Colour initialColour;
    juce::Colour toggleColour;
    juce::Colour toggleShiftColour;
    bool isShift;
    ShiftToggleSquareButton(juce::Colour initialColour, juce::Colour toggleColour,juce::Colour toggleShiftColour,bool isShift ,const char *image, size_t imagesize)
        : initialColour(initialColour), toggleColour(toggleColour),toggleShiftColour(toggleShiftColour), isShift(isShift),iconImage(image), iconImageSize(imagesize),  isToggled(false) {
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

    bool getToggleState() const {
        return isToggled;
    }

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {
        auto iconImageArea = juce::Rectangle<float>(0, 0, getWidth(), getHeight());
        auto drawAble = juce::Drawable::createFromImageData(iconImage, iconImageSize);
        drawAble->drawWithin(g, iconImageArea, juce::RectanglePlacement::xRight, 1.f);

        //		g.drawImage(iconImage, getLocalBounds().toFloat());
        g.fillAll(isToggled ? isShift ? toggleShiftColour : toggleColour : initialColour);
    }

    void setShift(bool mode) {

        isShift = mode;
    }

private:

    const char *iconImage;
    size_t iconImageSize;
    bool isToggled;
};