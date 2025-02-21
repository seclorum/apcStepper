#include "PluginProcessor.h"
#include "ProcessorEditor.h"

apcSequencerProcessor::apcSequencerProcessor() = default;
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
