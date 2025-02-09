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
#include <map>

//=============================================================================
class apcSequencerProcessor : public juce::AudioProcessor
{
public:
    apcSequencerProcessor() : link(120.0) // Initialize Ableton Link with default BPM
    {
        link.enable(true); // Enable Link
    }

    ~apcSequencerProcessor() override {}

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        // Corrected: Properly initialize the MIDI grid
        for (auto& row : midiGrid)
        {
            row.fill(false);
        }

        scrollOffset = 0; // Initialize scroll offset
        pageOffset = 0; // Initialize page offset
    }

    void releaseResources() override {}

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        buffer.clear();

        // Sync with Ableton Link
        auto timeline = link.captureAppTimeline();
        auto quantum = 4.0; // Bars per cycle
        auto phase = timeline.phaseAtTime(link.clock().micros(), quantum);
        auto beats = timeline.beatsAtTime(link.clock().micros(), quantum);

        // Output MIDI messages based on the current Link phase
        for (int column = 0; column < 16; ++column)
        {
            for (int row = 0; row < 24; ++row)
            {
                int adjustedColumn = column;
                int adjustedRow = (row + scrollOffset) % 24;

                // Fixed: Check for valid array index boundaries
                if (adjustedColumn >= 0 && adjustedColumn < 16 &&
                    adjustedRow >= 0 && adjustedRow < 24 &&
                    midiGrid[adjustedColumn][adjustedRow] &&
                    static_cast<int>(beats) % 16 == column)
                {
                    auto message = juce::MidiMessage::noteOn(1, mapRowColumnToNote(adjustedRow, adjustedColumn), (juce::uint8)127);
                    message.setTimeStamp(phase);
                    midiMessages.addEvent(message, 0);

                    auto noteOff = juce::MidiMessage::noteOff(1, mapRowColumnToNote(adjustedRow, adjustedColumn));
                    midiMessages.addEvent(noteOff, 10);
                }
            }
        }
    }

    void scrollGridUp()
    {
        scrollOffset = std::max(0, scrollOffset - 1);
    }

    void scrollGridDown()
    {
        scrollOffset = std::min(23, scrollOffset + 1); // Adjust for 24 rows
    }

    void jumpPageLeft()
    {
        pageOffset = std::max(0, pageOffset - 1);
    }

    void jumpPageRight()
    {
        pageOffset = std::min(1, pageOffset + 1);
    }

    juce::AudioProcessorEditor* createEditor() override { return new apcSequencerProcessorEditor(*this); }
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "apcSequencerPlugin"; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return true; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override {}
    const juce::String getProgramName(int index) override { return {}; }
    void changeProgramName(int index, const juce::String& newName) override {}

    void getStateInformation(juce::MemoryBlock& destData) override {}
    void setStateInformation(const void* data, int sizeInBytes) override {}

private:
    ableton::Link link;
    std::array<std::array<bool, 24>, 16> midiGrid; // Grid for 16 steps and 24 rows
    int scrollOffset = 0; // Scroll offset for rows
    int pageOffset = 0;  // Page offset for columns

    int mapRowColumnToNote(int row, int column)
    {
        return 36 + (row + column * 24); // Adjust for MIDI note mapping
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(apcSequencerProcessor);
};


