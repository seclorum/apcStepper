// Include JUCE headers
#include <JuceHeader.h>
#include <vector>
#include <algorithm>
#include <map>

//==============================================================================
class BasicAudioProcessor : public juce::AudioProcessor
{
public:
    BasicAudioProcessor() {}
    ~BasicAudioProcessor() override {}

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        midiGrid.fill(false); // Initialize the MIDI grid
    }

    void releaseResources() override {}

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        buffer.clear();

        // Process incoming MIDI messages
        for (const auto metadata : midiMessages)
        {
            const auto message = metadata.getMessage();
            if (message.isNoteOnOrOff())
            {
                int note = message.getNoteNumber();
                int row = mapNoteToRow(note);
                int column = mapNoteToColumn(note);

                if (row >= 0 && column >= 0)
                {
                    if (message.isNoteOn())
                        midiGrid[column][row] = true;
                    else if (message.isNoteOff())
                        midiGrid[column][row] = false;
                }
            }
        }

        // Generate MIDI output from the grid
        for (int column = 0; column < 8; ++column)
        {
            for (int row = 0; row < 12; ++row)
            {
                if (midiGrid[column][row])
                {
                    auto message = juce::MidiMessage::noteOn(1, mapRowColumnToNote(row, column), (juce::uint8)127);
                    message.setTimeStamp((double)column);
                    midiMessages.addEvent(message, (int)(column * (buffer.getNumSamples() / 8.0)));

                    // Add note-off for the same note later
                    auto noteOff = juce::MidiMessage::noteOff(1, mapRowColumnToNote(row, column));
                    noteOff.setTimeStamp((double)(column + 1));
                    midiMessages.addEvent(noteOff, (int)((column + 1) * (buffer.getNumSamples() / 8.0)));
                }
            }
        }
    }

    juce::AudioProcessorEditor* createEditor() override { return new BasicAudioProcessorEditor(*this); }
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "BasicProcessor"; }

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
    std::array<std::array<bool, 12>, 8> midiGrid; // Grid for 8 steps and 12 notes

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
        return rowToNote[row + column * 12];
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BasicAudioProcessor)
};

//==============================================================================
class BasicAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    BasicAudioProcessorEditor(BasicAudioProcessor& p) : juce::AudioProcessorEditor(&p), processor(p)
    {
        setSize(600, 400);

        // Create 8x12 grid of toggle buttons
        for (int row = 0; row < 12; ++row)
        {
            for (int col = 0; col < 8; ++col)
            {
                auto button = std::make_unique<juce::ToggleButton>("Button R" + juce::String(row) + " C" + juce::String(col));
                button->setBounds(50 + col * 60, 50 + row * 30, 50, 20);
                button->onClick = [this, row, col]() {
                    toggleGridState(row, col);
                };
                addAndMakeVisible(*button);
                gridButtons.push_back(std::move(button));
            }
        }
    }

    ~BasicAudioProcessorEditor() override {}

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
        processor.midiGrid[col][row] = !processor.midiGrid[col][row];
    }

    BasicAudioProcessor& processor;
    std::vector<std::unique_ptr<juce::ToggleButton>> gridButtons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BasicAudioProcessorEditor)
};

//==============================================================================
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicAudioProcessor();
}
