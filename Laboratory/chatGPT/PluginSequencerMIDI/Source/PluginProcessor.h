// Include JUCE headers
#include <JuceHeader.h>
#include <vector>
#include <algorithm>

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
                int velocity = message.getVelocity();

                // Map MIDI note to grid (12 rows for notes, 8 columns for steps)
                int row = note % 12;
                int column = (note / 12) % 8;

                if (message.isNoteOn())
                    midiGrid[column][row] = true;
                else if (message.isNoteOff())
                    midiGrid[column][row] = false;
            }
        }

        // Generate MIDI output from the grid
        for (int column = 0; column < 8; ++column)
        {
            for (int row = 0; row < 12; ++row)
            {
                if (midiGrid[column][row])
                {
                    auto message = juce::MidiMessage::noteOn(1, row + 12 * column, (juce::uint8)127);
                    message.setTimeStamp((double)column);
                    midiMessages.addEvent(message, (int)(column * (buffer.getNumSamples() / 8.0)));

                    // Add note-off for the same note later
                    auto noteOff = juce::MidiMessage::noteOff(1, row + 12 * column);
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

