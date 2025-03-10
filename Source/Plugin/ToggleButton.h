// ToggleSquare.h
#pragma once

#include <BinaryData.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

class ToggleButton : public juce::TextButton {
public:
    juce::Colour initialColour;
    juce::Colour toggleColour;

    ToggleButton(juce::Colour initialColour, juce::Colour toggleColour)
        : initialColour(initialColour), toggleColour(toggleColour),
          isToggled(false) {
        setClickingTogglesState(true);
        setColour(juce::TextButton::buttonColourId, initialColour);
    }

    void clicked() override {
        isToggled = !isToggled;
        repaint();
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

            auto drawAble = juce::Drawable::createFromImageData(BinaryData::button_svg, BinaryData::button_svgSize);
            drawAble->drawWithin(g, iconImageArea, juce::RectanglePlacement::xRight, 1.f);

        g.fillAll(isToggled ? toggleColour : initialColour);
    }


private:


    bool isToggled;
};
