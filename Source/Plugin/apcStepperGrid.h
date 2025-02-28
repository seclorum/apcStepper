//
// Created by Jay Vaughan on 26.02.25.
//


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


#define APCSTEPPER_APCSTEPPERGRID_H
#define APCSTEPPER_APCSTEPPERGRID_H

class apcStepperMainProcessor;

#include <juce_gui_basics/juce_gui_basics.h>
class ToggleSquare : public juce::Component
{
public:
    ToggleSquare()
    {
        setOpaque(true);
        setColour(toggleColourId, juce::Colours::grey);
    }

    void mouseDown(const juce::MouseEvent&) override
    {
        isToggled = !isToggled;
        setColour(toggleColourId, isToggled ? juce::Colours::red : juce::Colours::grey);
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(findColour(toggleColourId));
        g.setColour(juce::Colours::white);
        g.drawRect(getLocalBounds(), 2);
    }


private:
    bool isToggled = false;

    enum ColourIds
    {
        toggleColourId = 0x20001000
    };
};

class apcStepperGrid : public juce::AudioProcessorEditor {
public:
    static constexpr int rows = 8;
    static constexpr int cols = 8;
    static constexpr int padding = 5;
    // Constructor: initializes the editor and attaches UI components to processor parameters.
    apcStepperGrid(apcStepperMainProcessor &p)
    : AudioProcessorEditor(&p), processor(p) {
        for (int i = 0; i < rows * cols; ++i)
        {
            auto square = std::make_unique<ToggleSquare>();
            addAndMakeVisible(*square);
            squares.add(std::move(square));
        }
    }


    void resized() override
    {
        float cellSize = juce::jmin(getWidth() / cols, getHeight() / rows);

        juce::FlexBox flexBox;
        flexBox.flexDirection = juce::FlexBox::Direction::column;
        flexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        flexBox.alignItems = juce::FlexBox::AlignItems::stretch;

        for (int row = 0; row < rows; ++row)
        {
            juce::FlexBox rowBox;
            rowBox.flexDirection = juce::FlexBox::Direction::row;
            rowBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
            rowBox.alignItems = juce::FlexBox::AlignItems::stretch;

            for (int col = 0; col < cols; ++col)
            {
                int index = row * cols + col;
                squares[index]->setBounds(col * cellSize, row * cellSize, cellSize, cellSize);
                rowBox.items.add(juce::FlexItem(*squares[index]).withWidth(cellSize).withHeight(cellSize).withFlex(1));
            }

            flexBox.items.add(juce::FlexItem(rowBox).withWidth(getWidth()).withHeight(cellSize));
        }

        flexBox.performLayout(getLocalBounds().toFloat());
    }
    
private:
    apcStepperMainProcessor &processor;
    juce::OwnedArray<ToggleSquare> squares;
};
