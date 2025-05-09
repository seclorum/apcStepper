#pragma once
#include <juce_analytics/juce_analytics.h>
#include <juce_animation/juce_animation.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_box2d/juce_box2d.h>
#include <juce_core/juce_core.h>
#include <juce_cryptography/juce_cryptography.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_javascript/juce_javascript.h>
#include <juce_midi_ci/juce_midi_ci.h>
#include <juce_opengl/juce_opengl.h>
#include <juce_osc/juce_osc.h>
#include <juce_product_unlocking/juce_product_unlocking.h>
#include <juce_video/juce_video.h>

#include <array>

#ifdef USE_MELATONIN_INSPECTOR
#include <melatonin_inspector/melatonin_inspector.h>
#endif

#include "trackDeckMainProcessor.h"
#include "trackDeckMainEditor.h"
#include "apcShiftToggleParameterButton.h"
#include "apcToggleButton.h"
#include "ToggleIconButton.h"

class trackDeckMainProcessor;
using namespace juce;
class apcTempoPanel : public juce::AudioProcessorEditor {
public:
    apcTempoPanel(trackDeckMainProcessor &p)
        : AudioProcessorEditor(&p), processor(p) {
        tempoFlexPanel.flexDirection = juce::FlexBox::Direction::column;
        tempoFlexPanel.justifyContent = juce::FlexBox::JustifyContent::center;
        tempoFlexPanel.alignItems = juce::FlexBox::AlignItems::stretch;
        tempoSlider = std::make_unique<juce::Slider>();
        tempoLabel = std::make_unique<juce::Label>();
        tempoFlexPanel.items.add(FlexItem(*tempoSlider).withFlex(1));
        tempoFlexPanel.items.add(FlexItem(*tempoLabel).withFlex(1));
        tempoSlider->setSliderStyle(Slider::LinearHorizontal);
        tempoSlider->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);

        tempoSlider->setRange(20.0f, 300.0f, 1.0f); // Typical tempo range
        tempoSlider->setVisible(true);
        addAndMakeVisible(tempoSlider.get());

        // Editable Tempo Label
        tempoLabel->setEditable(true);
        tempoLabel->setJustificationType(Justification::centred);
        tempoLabel->setColour(Label::textColourId, Colours::white);
        tempoLabel->setColour(Label::backgroundColourId, Colours::black);
        tempoLabel->setText(juce::String(tempoSlider->getValue()), dontSendNotification);
        tempoLabel->onTextChange = [this] {
            float newTempo = tempoLabel->getText().getFloatValue();
            tempoSlider->setValue(newTempo, juce::sendNotification);
        };
        addAndMakeVisible(tempoLabel.get());

        tempoAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                processor.getParameters(), "tempo", *tempoSlider);

        // Sync tempo label with slider
        tempoSlider->onValueChange = [this] {
            tempoLabel->setText(juce::String(tempoSlider->getValue()), juce::dontSendNotification);
            // processor.setTempo(roundToInt(tempoSlider->getValue()));
        };

        APCLOG("apcTempoPanel initialized...");

    }

    void resized() override {
        tempoFlexPanel.performLayout(getLocalBounds());

    }
private:

    std::unique_ptr<juce::Slider> tempoSlider; // Slider for tempo
    std::unique_ptr<juce::Label> tempoLabel; // Editable tempo label

    // Attachments used to bind to parameters in the processor
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tempoAttachment;
    juce::FlexBox tempoFlexPanel;
    trackDeckMainProcessor &processor;

#ifdef USE_MELATONIN_INSPECTOR
    #warning "MELATONIN INSPECTOR IS CONFIGURED TO BE INCLUDED IN BUILD"
    melatonin::Inspector inspector { *this, true };
#endif

};

