#pragma once

// !J! Just include all the JUCE things.  Makes it easier to navigate... but slower to build
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


#include <vector>
#include <memory>

#include "MIDIArpeggiatorEditor.h"
#include "melatonin_inspector/melatonin_inspector.h"

// Forward declarations to avoid unnecessary includes
class apcSequencerProcessor;
class apcSequencerProcessorEditor;

using namespace juce;

//==============================================================================
// Simple About Box Component
class apcAboutBox final : public Component
{
public:
    apcAboutBox()
    {
        addAndMakeVisible(aboutLabel);
    }

    void paint(Graphics& g) override {
        g.fillAll(juce::Colours::orange); // Ensure background is white
    }

    void resized() override
    {
        aboutLabel.setBounds(getLocalBounds().reduced(10));
    }

private:
    Label aboutLabel{{}, {"Hello, this is Jay and Tom!"}};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(apcAboutBox)
};

//==============================================================================
// Editor for the APC Sequencer Processor
class apcSequencerProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit apcSequencerProcessorEditor(apcSequencerProcessor& processor);
    ~apcSequencerProcessorEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    apcSequencerProcessor& processor;

    // Melatonin Inspector for debugging
    melatonin::Inspector inspector{ *this };

    // Tabbed UI Component
    std::unique_ptr<TabbedComponent> tabbedComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(apcSequencerProcessorEditor)
};
