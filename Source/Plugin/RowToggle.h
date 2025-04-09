#pragma once

#include "apcToggleButton.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

class RowToggle : public apcToggleButton {
public:
    juce::Colour initialColour;
    juce::Colour toggleColour;

    RowToggle(juce::Colour initialColour, juce::Colour toggleColour)
       : apcToggleButton(initialColour, toggleColour){
        setButtonText("Velocity");
        // Initialize SharedResourcePointer with a lambda that creates the Typeface
        auto temp = juce::Typeface::createSystemTypefaceFor(BinaryData::ConthraxSemiBold_otf, BinaryData::ConthraxSemiBold_otfSize);
        if (temp != nullptr)
        {
            typeface = juce::Typeface::Ptr(temp.get()); // Transfer ownership to Typeface::Ptr
            customFont = juce::Font(typeface);
            //customFont.setHeight(14.0f);;
        }


    }

    void clicked() override {
        isToggled = !isToggled;
        setButtonText(getToggleState() ? "Length" : "Velocity");
    }

    void resized() override {
       repaint();
    }

    void paintOverChildren(juce::Graphics& g) override
    {


        // Draw the button text centered and scaled
        g.setColour(juce::Colours::darkgrey);
        g.setFont(customFont); // Scale font size
        juce::Rectangle<float> textBounds = getLocalBounds().toFloat();
        g.drawFittedText(getButtonText(), getLocalBounds().reduced(5,0), juce::Justification::centred, 1); // Draw centered and scale if needed

    }

private:

    juce::Font customFont;
    juce::Typeface::Ptr typeface;
    bool isToggled;
};