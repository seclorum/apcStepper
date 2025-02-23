#include "apcSequencerProcessor.h"
#include "apcSequencerProcessorEditor.h"


apcSequencerProcessor::apcSequencerProcessor() = default;

// !J!  TODO: figure out bus layout
#if 0
apcSequencerProcessor::apcSequencerProcessor()
    : juce::AudioProcessor(BusesProperties()
        .withOutput("MIDI Out", juce::AudioChannelSet::disabled(), true))  // ✅ No audio processing
{
}
#endif


apcSequencerProcessor::~apcSequencerProcessor() = default;

void apcSequencerProcessor::prepareToPlay(double, int)
{
    for (auto& row : midiGrid)
    {
        row.fill(false);
    }
    scrollOffset = 0;
    pageOffset = 0;
}

void apcSequencerProcessor::releaseResources() {}

void apcSequencerProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    buffer.clear();

#if 0
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
                // static_cast<int>(beats) % 16 == column)
            {
                auto message = juce::MidiMessage::noteOn(1, mapRowColumnToNote(adjustedRow, adjustedColumn), (juce::uint8)127);
//                 message.setTimeStamp(phase);
//                 midiMessages.addEvent(message, 0);

                auto noteOff = juce::MidiMessage::noteOff(1, mapRowColumnToNote(adjustedRow, adjustedColumn));
//                midiMessages.addEvent(noteOff, 10);
            }
        }
#endif

}

void apcSequencerProcessor::scrollGridUp() { scrollOffset = std::max(0, scrollOffset - 1); }
void apcSequencerProcessor::scrollGridDown() { scrollOffset = std::min(23, scrollOffset + 1); }
void apcSequencerProcessor::jumpPageLeft() { pageOffset = std::max(0, pageOffset - 1); }
void apcSequencerProcessor::jumpPageRight() { pageOffset = std::min(1, pageOffset + 1); }

juce::AudioProcessorEditor* apcSequencerProcessor::createEditor()
{
    return new apcSequencerProcessorEditor(*this);
}

void apcSequencerProcessor::getStateInformation(juce::MemoryBlock&) {}
void apcSequencerProcessor::setStateInformation(const void*, int) {}

int apcSequencerProcessor::mapRowColumnToNote(int row, int column)
{
    return 36 + (row + column * 24);
}

bool apcSequencerProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::disabled();
}


// This function is required for JUCE plugins!
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new apcSequencerProcessor();
}

