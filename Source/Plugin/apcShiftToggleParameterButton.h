#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

class apcShiftToggleParameterButton : public juce::TextButton, private juce::RangedAudioParameter::Listener {
public:
    apcShiftToggleParameterButton(std::string buttonName, int track, juce::Colour initialColour, juce::Colour toggleColour, juce::Colour toggleShiftColour, bool isShift, apcStepperMainProcessor& p)
        : juce::TextButton(buttonName), buttonName(buttonName), track(track), initialColor(initialColour), toggleColour(toggleColour), toggleShiftColour(toggleShiftColour), isShift(isShift), processor(p) {
        setClickingTogglesState(true);
        current_attachment = processor.getParameters().getParameter("r" + processor.addLeadingZeros(track));
        if (current_attachment) {
            current_attachment->addListener(this);
            setToggleState(current_attachment->getValue() > 0.5f, juce::dontSendNotification); // Sync initial state
        }
    }

    ~apcShiftToggleParameterButton() override {
        if (current_attachment)
            current_attachment->removeListener(this);
    }

    void clicked() override {
        juce::TextButton::clicked(); // Toggles state
        if (current_attachment) {
            float newValue = getToggleState() ? 1.0f : 0.0f;
            current_attachment->setValueNotifyingHost(newValue); // Update parameter
        }
    }

    void resized() override {
        // No-op: Let parent (FlexBox) set bounds
    }

    void parameterValueChanged(int parameterIndex, float newValue) override {
        isCurrent = (newValue > 0.5f);
        processor.currentSelect = track;
        setToggleState(isCurrent, juce::dontSendNotification); // Sync with parameter
        repaint();
    }

    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {
        // No-op
    }

    void paint(juce::Graphics& g) override {
        juce::Colour fillColour = getToggleState() ? (isShift ? toggleShiftColour : toggleColour) : initialColor;
        g.fillAll(fillColour);
        auto drawable = juce::Drawable::createFromImageData(BinaryData::button_svg, BinaryData::button_svgSize);
        if (drawable)
            drawable->drawWithin(g, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit, 1.0f);
    }

    void setShift(bool mode) {
        isShift = mode;
        repaint(); // Update color
    }

    void setColourToggle(juce::Colour colour) {
        toggleColour = colour;
        repaint(); // Reflect change
    }

private:
    std::string buttonName;
    int track;
    bool isShift = false;
    bool isCurrent = false;
    juce::Colour initialColor;
    juce::Colour toggleColour;
    juce::Colour toggleShiftColour;
    juce::RangedAudioParameter* current_attachment = nullptr;
    apcStepperMainProcessor& processor;
};