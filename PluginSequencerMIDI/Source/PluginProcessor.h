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
