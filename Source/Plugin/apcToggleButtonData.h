#pragma once

#include <BinaryData.h>
#include <juce_audio_processors/utilities/juce_AudioProcessorValueTreeState.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "apcStepperMainProcessor.h"
#include "apcStepperMainEditor.h"
#include "ShiftToggleButton.h"
#include "apcToggleButton.h"
#include "ToggleIconButton.h"

class apcStepperMainProcessor;
using namespace juce;
class apcToggleButtonData : public juce::TextButton {
public:

    apcToggleButtonData(juce::Colour initialColour, buttonName juce::Colour toggleColour,apcStepperMainProcessor &p)

        : juce::TextButton(""),initialColour(initialColour), buttonName(buttonName),toggleColour(toggleColour),AudioProcessorEditor(&p), processor(p) , isToggled(false) {
        setClickingTogglesState(true); // Enables automatic toggling
        setColour(juce::TextButton::buttonColourId, initialColour);
        button_attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
           processor.getParameters(), buttonName, tempoSlider);

        // Sync tempo label with slider

    }
    void onValueChange() override { syncTempo(); };
    bool buttonStateChanged() override {
        isToggled = getToggleState(); // Sync state
        repaint();
        return isToggled;
    }
    void setColourToggle(const juce::Colour newColour) {
        toggleColour = newColour; // Update toggle color
        repaint(); // Ensure UI updates
    }
    void syncTempo()
    {
        processor.setTempo(roundToInt(getToggleState()));
    }
    void paintButton(juce::Graphics &g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {
        g.fillAll(isToggled ? toggleColour : initialColour);

        if (auto drawable = juce::Drawable::createFromImageData(BinaryData::button_svg, BinaryData::button_svgSize)) {
            drawable->drawWithin(g, drawable->getDrawableBounds(), juce::RectanglePlacement::fillDestination, 1.0f);
        }
    }

private:
    juce::Colour initialColour;
    juce::Colour toggleColour;
    bool isToggled;
    std::pmr::string buttonName;
    apcStepperMainProcessor &processor;;
    std::unique_ptr<juce::AudioProcessorValueTreeState::BoolAttachment> button_attachment;


};
