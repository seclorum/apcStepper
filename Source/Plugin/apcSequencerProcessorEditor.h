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

class apcSequencerProcessor;
class apcSequencerProcessorEditor;

using namespace juce;

//==============================================================================
class apcAboutBox final : public Component
{
public:
    apcAboutBox() {
        setOpaque (true);
        addAndMakeVisible (aboutLabel);
    }

private:
    Label aboutLabel{{},{"Hello this is Jay and Tom!"}};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (apcAboutBox)

};


//==============================================================================
struct DemoTabbedComponent final : public TabbedComponent
{
    DemoTabbedComponent (bool isRunningComponentTransformsDemo)
        : TabbedComponent (TabbedButtonBar::TabsAtTop)
    {
        auto colour = juce::Colour::fromRGB(128, 0, 128);

        // addTab ("Tables",      colour, new apcSequencerProcessorEditor(),                          true);
//        addTab ("Arpeggiator", colour, new MIDIArpeggiatorEditor(),                          true);
        addTab ("About", colour, new apcAboutBox(),true);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DemoTabbedComponent)
};


class apcSequencerProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit apcSequencerProcessorEditor(apcSequencerProcessor& processor);
    ~apcSequencerProcessorEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    apcSequencerProcessor& processor;


    DemoTabbedComponent apcTabs = new DemoTabbedComponent(true);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(apcSequencerProcessorEditor)
};
