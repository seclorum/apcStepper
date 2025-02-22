
// MIDIArpeggiator.cpp
#include "MIDIArpeggiator.h"

MIDIArpeggiator::MIDIArpeggiator()
    : AudioProcessor(BusesProperties().withInput("MIDI Input", juce::AudioChannelSet::disabled(), true)
                                    .withOutput("MIDI Output", juce::AudioChannelSet::disabled(), true)) {}

const juce::String MIDIArpeggiator::getName() const { return "MIDI Arpeggiator"; }
bool MIDIArpeggiator::acceptsMIDI() const { return true; }
bool MIDIArpeggiator::producesMIDI() const { return true; }
double MIDIArpeggiator::getTailLengthSeconds() const { return 0.0; }
int MIDIArpeggiator::getNumPrograms() { return 1; }
int MIDIArpeggiator::getCurrentProgram() { return 0; }
void MIDIArpeggiator::setCurrentProgram(int) {}
const juce::String MIDIArpeggiator::getProgramName(int) { return {}; }
void MIDIArpeggiator::changeProgramName(int, const juce::String&) {}
void MIDIArpeggiator::prepareToPlay(double newSampleRate, int) {
    sampleRate = newSampleRate;
    samplesPerArpeggioStep = static_cast<int>((60.0 / arpeggioSpeed) * sampleRate / 4);
}
void MIDIArpeggiator::releaseResources() {}
void MIDIArpeggiator::processBlock(juce::AudioBuffer<float>&, juce::MIDIBuffer& midiMessages) {
    juce::MIDIBuffer processedMIDI;
    for (const auto meta : midiMessages) {
        auto message = meta.getMessage();
        if (message.isNoteOn()) {
            heldNotes.push_back(message.getNoteNumber());
        } else if (message.isNoteOff()) {
            heldNotes.erase(std::remove(heldNotes.begin(), heldNotes.end(), message.getNoteNumber()), heldNotes.end());
        }
    }
    midiMessages.clear();
    if (!heldNotes.empty()) {
        sampleCounter++;
        if (sampleCounter >= samplesPerArpeggioStep) {
            sampleCounter = 0;
            lastNoteIndex = (lastNoteIndex + 1) % heldNotes.size();
            processedMIDI.addEvent(juce::MIDIMessage::noteOn(1, heldNotes[lastNoteIndex], (juce::uint8)100), 0);
        }
    }
    midiMessages.swapWith(processedMIDI);
}
bool MIDIArpeggiator::hasEditor() const { return false; }
juce::AudioProcessorEditor* MIDIArpeggiator::createEditor() { return nullptr; }
void MIDIArpeggiator::getStateInformation(juce::MemoryBlock&) {}
void MIDIArpeggiator::setStateInformation(const void*, int) {}

//juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
//    return new MIDIArpeggiator();
//}
