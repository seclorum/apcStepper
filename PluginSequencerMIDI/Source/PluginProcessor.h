#include <JuceHeader.h>
#include <vector>
#include <algorithm>
#include <map>
#include <ableton/Link.hpp>

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
        midiGrid.fill(false); // Initialize the MIDI grid
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

                if (midiGrid[adjustedColumn][adjustedRow] && static_cast<int>(beats) % 16 == column)
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

    juce::AudioProcessorEditor* createEditor() override { return new BasicAudioProcessorEditor(*this); }
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
    std::array<std::array<bool, 24>, 16> midiGrid; // Grid for 16 steps (2 pages) and 24 rows (2 octaves)
    int scrollOffset = 0; // Scroll offset for rows
    int pageOffset = 0;  // Page offset for columns

    int mapNoteToRow(int note)
    {
        static std::map<int, int> noteToRowMap = {
            {0x38, 0}, {0x39, 1}, {0x3A, 2}, {0x3B, 3}, {0x3C, 4}, {0x3D, 5}, {0x3E, 6}, {0x3F, 7},
            {0x30, 0}, {0x31, 1}, {0x32, 2}, {0x33, 3}, {0x34, 4}, {0x35, 5}, {0x36, 6}, {0x37, 7},
            {0x28, 0}, {0x29, 1}, {0x2A, 2}, {0x2B, 3}, {0x2C, 4}, {0x2D, 5}, {0x2E, 6}, {0x2F, 7},
            {0x20, 0}, {0x21, 1}, {0x22, 2}, {0x23, 3}, {0x24, 4}, {0x25, 5}, {0x26, 6}, {0x27, 7},
            {0x18, 0}, {0x19, 1}, {0x1A, 2}, {0x1B, 3}, {0x1C, 4}, {0x1D, 5}, {0x1E, 6}, {0x1F, 7},
            {0x10, 0}, {0x11, 1}, {0x12, 2}, {0x13, 3}, {0x14, 4}, {0x15, 5}, {0x16, 6}, {0x17, 7},
            {0x08, 0}, {0x09, 1}, {0x0A, 2}, {0x0B, 3}, {0x0C, 4}, {0x0D, 5}, {0x0E, 6}, {0x0F, 7},
            {0x00, 0}, {0x01, 1}, {0x02, 2}, {0x03, 3}, {0x04, 4}, {0x05, 5}, {0x06, 6}, {0x07, 7}
        };

        auto it = noteToRowMap.find(note);
        return (it != noteToRowMap.end()) ? it->second : -1;
    }

    int mapNoteToColumn(int note)
    {
        return (note >= 0x30) ? (note / 0x30) % 8 : (note / 0x08) % 8;
    }

    int mapRowColumnToNote(int row, int column)
    {
        static std::vector<int> rowToNote = {
            0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
            0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
            0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
            0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
        };
        return rowToNote[row + column * 24]; // Adjust for 24 rows
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(apcSequencerProcessor)
};

//==============================================================================
class apcSequencerProcessorEditor : public juce::AudioProcessorEditor
{
public:
    apcSequencerProcessorEditor(apcSequencerProcessor& p) : juce::AudioProcessorEditor(&p), processor(p)
    {
        setSize(800, 600); // Adjust size for larger grid

        // Create 16x24 grid of toggle buttons
        for (int row = 0; row < 24; ++row)
        {
            for (int col = 0; col < 16; ++col)
            {
                auto button = std::make_unique<juce::ToggleButton>("Button R" + juce::String(row) + " C" + juce::String(col));
                button->setBounds(50 + col * 40, 50 + row * 20, 30, 15);
                button->onClick = [this, row, col]() {
                    toggleGridState(row, col);
                };
                addAndMakeVisible(*button);
                gridButtons.push_back(std::move(button));
            }
        }

        // Add track buttons
        for (int i = 0; i < 8; ++i)
        {
            auto trackButton = std::make_unique<juce::TextButton>("Track Button " + juce::String(i + 1));
            trackButton->setBounds(10, 50 + i * 40, 100, 30);
            trackButton->onClick = [this, i]() {
                handleTrackButtonPress(i);
            };
            addAndMakeVisible(*trackButton);
            trackButtons.push_back(std::move(trackButton));
        }

        // Add scene launch buttons
        for (int i = 0; i < 8; ++i)
        {
            auto sceneButton = std::make_unique<juce::TextButton>("Scene Launch " + juce::String(i + 1));
            sceneButton->setBounds(50 + i * 60, 500, 100, 30);
            sceneButton->onClick = [this, i]() {
                handleSceneButtonPress(i);
            };
            addAndMakeVisible(*sceneButton);
            sceneButtons.push_back(std::move(sceneButton));
        }
    }

    ~apcSequencerProcessorEditor() override {}

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
    }

    void resized() override
    {
        // Adjust layout dynamically if needed
    }

private:
    void toggleGridState(int row, int col)
    {
        int adjustedCol = (col + processor.pageOffset * 8) % 16;
        int adjustedRow = (row + processor.scrollOffset) % 24;
        processor.midiGrid[adjustedCol][adjustedRow] = !processor.midiGrid[adjustedCol][adjustedRow];
    }

    void handleTrackButtonPress(int index)
    {
        if (index == 4) processor.scrollGridUp();
        else if (index == 5) processor.scrollGridDown();
        else if (index == 6) processor.jumpPageLeft();
        else if (index == 7) processor.jumpPageRight();
    }

    void handleSceneButtonPress(int index)
    {
        juce::Logger::writeToLog("Scene Launch " + juce::String(index + 1) + " pressed");
    }

    apcSequencerProcessor& processor;
    std::vector<std::unique_ptr<juce::ToggleButton>> gridButtons;
    std::vector<std::unique_ptr<juce::TextButton>> trackButtons;
    std::vector<std::unique_ptr<juce::TextButton>> sceneButtons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(apcSequencerProcessorEditor)
};

//==============================================================================
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new apcSequencerProcessor();
}
