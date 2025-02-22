// MIDIArpeggiator.h
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>

class MIDIArpeggiator : public juce::AudioProcessor {
public:
    MIDIArpeggiator();
    const juce::String getName() const override;
    bool acceptsMIDI() const override;
    bool producesMIDI() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int) override;
    void changeProgramName(int, const juce::String&) override;
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MIDIBuffer& midiMessages) override;
    bool hasEditor() const override;
    juce::AudioProcessorEditor* createEditor() override;
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    
private:
    std::vector<int> heldNotes;
    int lastNoteIndex = 0;
    double sampleRate = 44100.0;
    int arpeggioSpeed = 120; // BPM
    int samplesPerArpeggioStep = 0;
    int sampleCounter = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIArpeggiator)
};
