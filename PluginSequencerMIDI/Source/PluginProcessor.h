#include <JuceHeader.h>
#include <vector>
#include <algorithm>
#include <map>
// #include <ableton/Link.hpp>

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(apcSequencerProcessor)
};

//==============================================================================
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new apcSequencerProcessor();
}
