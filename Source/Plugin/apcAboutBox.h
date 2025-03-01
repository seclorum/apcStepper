//
// Created by Jay Vaughan on 26.02.25.
//

#ifndef APCSTEPPER_APCABOUTBOX_H
#define APCSTEPPER_APCABOUTBOX_H

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

#include "BinaryData.h"

using namespace juce;

//==============================================================================
// Simple About Box Component
class apcAboutBox final : public Component
{
public:
    apcAboutBox()
    {
        auto imageInputStream = std::make_unique<juce::MemoryInputStream>(BinaryData::cuttlefish_jpg, BinaryData::cuttlefish_jpgSize, false);
        cuttlefishImage = juce::PNGImageFormat().decodeImage(*imageInputStream);
        addAndMakeVisible(aboutLabel);
    }

    void paint(Graphics& g) override {
        g.fillAll(juce::Colours::orange); // Ensure background is white

        if (cuttlefishImage.isValid())
        {
            auto imageBounds = juce::Rectangle<int>(20, 40, 260, 180); // Define image area
            g.drawImage(cuttlefishImage, imageBounds.toFloat());
        }

    }

    void resized() override
    {
        aboutLabel.setBounds(getLocalBounds().reduced(10));
    }

private:
    Label aboutLabel{{}, {"Hello, this is Jay and Tom!"}};
    Image cuttlefishImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(apcAboutBox)
};


#endif //APCSTEPPER_APCABOUTBOX_H
