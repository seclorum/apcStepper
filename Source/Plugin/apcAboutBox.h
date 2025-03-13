//
// Created by Jay Vaughan on 26.02.25.
//

#ifndef APCSTEPPER_APCABOUTBOX_H
#define APCSTEPPER_APCABOUTBOX_H

#include "Common.h"

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
        APCLOG("AboutBox resize message");

    }

private:
    Label aboutLabel{{}, {"Hello, this is Jay and Tom!"}};
    Image cuttlefishImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(apcAboutBox)
};


#endif //APCSTEPPER_APCABOUTBOX_H
