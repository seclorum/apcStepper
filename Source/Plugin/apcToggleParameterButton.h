#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include <BinaryData.h>

#include "trackDeckMainProcessor.h"

//#include "trackDeckMainEditor.h"
//#include "ShiftToggleButton.h"
//#include "apcToggleButton.h"
//#include "ToggleIconButton.h"
static const int apcPARAMETER_V1 = 0x01;

class apcToggleParameterButton : public juce::TextButton, private juce::AudioProcessorParameter::Listener {
public:
    apcToggleParameterButton(std::string buttonName, int step, int track, juce::Colour initialColour, trackDeckMainProcessor &p)
        : juce::TextButton(buttonName), buttonName(buttonName), step(step), track(track), initialColour(initialColour),
           processor(p), isToggled(false) {

        setClickingTogglesState(true); // Enables automatic toggling
        setColour(juce::TextButton::buttonColourId, initialColour);

        button_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            processor.getParameters(), buttonName, *this);
        current_attachment = processor.getParameters().getParameter("c" + processor.addLeadingZeros(step));
        // Example ID
        if (current_attachment)
            current_attachment->addListener(this);
        currentColor = Colours::lightskyblue;
        instrument_attachment = processor.getParameters().getParameter("i" + processor.addLeadingZeros(step));
        // Example ID
        if (instrument_attachment)
            instrument_attachment->addListener(this);
        currentColor = Colours::lightskyblue;
        instrument_attachment->setValue(track);
    }

    ~apcToggleParameterButton() override {
        if (current_attachment)
            current_attachment->removeListener(this);
        if (instrument_attachment)
            instrument_attachment->removeListener(this);
    }

    void parameterValueChanged(int parameterIndex, float newValue) override {
        if (current_attachment->getParameterIndex()==parameterIndex) {
            isCurrent = (newValue > 0.5f); // Update current state
        }
        if (instrument_attachment->getParameterIndex()==parameterIndex) {
            track = newValue; // Update current std::to_string(state
            APCLOG(std::to_string(newValue));
        }
        repaint();
    }

    void clicked() override {
        isToggled = getToggleState();

        int midiNote = mapRowColumnToNote(step, track); // Map row index to MIDI notes (C1 and up)
        // !J! confirms that we can access MIDI from here:
        //isToggled ? processor.midiOutput->sendMessageNow(juce::MidiMessage::noteOn(6, midiNote, (juce::uint8) 55))
        //: processor.midiOutput->sendMessageNow(juce::MidiMessage::noteOn(6, midiNote, (juce::uint8) 00));
    }

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {
        auto bounds = getLocalBounds().toFloat();
        float h = bounds.getHeight();
        float w = bounds.getWidth();
        float y_split = h / 4.0f; // Split at 1/5 height
        float cornerRadius = std::min(h, w) * 0.1f; // 10% of smaller dimension for symmetric corners

        // Save graphics state
        g.saveState();



        g.setColour(rowColours[track]);
        g.fillRoundedRectangle(bounds, cornerRadius);
        g.restoreState();
        g.saveState();
        g.reduceClipRegion(bounds.getX(), bounds.getY(), w, h - y_split);
        g.setColour(isToggled ? juce::Colours::lightgrey : juce::Colours::darkgrey);
        g.fillRoundedRectangle(bounds, cornerRadius);

        g.restoreState();

        // Draw SVG if available
        auto drawable = juce::Drawable::createFromImageData(BinaryData::button_svg, BinaryData::button_svgSize);
        if (drawable)
            drawable->drawWithin(g, bounds, juce::RectanglePlacement::stretchToFit, 1.0f);
    }
    void resized() override {
    }

    int mapRowColumnToNote(int step, int track) {
        return (step + track * 8);
    }

    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {
        // No-op (empty implementation is fine)
    }

private:
    juce::Colour initialColour;
    juce::Colour toggleColour;
    juce::Colour currentColor;
    bool isToggled;
    bool isCurrent;
    juce::String buttonName;
    trackDeckMainProcessor &processor;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> button_attachment;
    juce::AudioProcessorParameter *current_attachment = nullptr;
    juce::AudioProcessorParameter *instrument_attachment = nullptr;
    int step;
    int track;
    juce::Colour pureRed     = juce::Colour(0xffFF0000); // Pure red
    juce::Colour brightGreen = juce::Colour(0xff00FF00); // Bright green
    juce::Colour pureBlue    = juce::Colour(0xff0000FF); // Pure blue
    juce::Colour vividYellow = juce::Colour(0xffFFFF00); // Bright yellow
    juce::Colour magenta     = juce::Colour(0xffFF4CFF); // Vivid magenta
    juce::Colour hotPink     = juce::Colour(0xffFF005D); // Hot pink
    juce::Colour orange      = juce::Colour(0xffFF7F00); // Bright orange
    juce::Colour neonGreen   = juce::Colour(0xff3BFF26); // Neon green
    juce::Colour deepViolet  = juce::Colour(0xff3F00FF); // Deep violet
    juce::Colour lightGreen  = juce::Colour(0xff59FF71); // Light green
    juce::Colour skyBlue     = juce::Colour(0xff00A9FF); // Sky blue
    juce::Colour fieryOrange = juce::Colour(0xffFF4A00); // Fiery orange
    juce::Colour brightPurple = juce::Colour(0xffD31DFF); // Bright purple
    juce::Colour goldenYellow = juce::Colour(0xffFFE126); // Golden yellow
    juce::Colour limeGreen   = juce::Colour(0xff90FF00); // Lime green
    juce::Colour coralPink   = juce::Colour(0xffFF4C87); // Coral pink

    juce::Array<juce::Colour> rowColours = {
        pureRed, neonGreen, pureBlue, vividYellow,
        magenta, skyBlue, orange, lightGreen,
        deepViolet, hotPink, fieryOrange, limeGreen,
        brightPurple, goldenYellow, coralPink, brightGreen
    };
};
