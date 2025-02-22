// MIDIArpeggiatorProcessor.h
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>

class MIDIArpeggiatorProcessor : public juce::AudioProcessor {
public:
    MIDIArpeggiatorProcessor();
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int) override;
    void changeProgramName(int, const juce::String&) override;
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer& midiMessages) override;

	// !J! Stub 'through' function
	static void processBlockMIDIThrough(juce::AudioBuffer<float> &, juce::MidiBuffer &midiMessages);

	bool hasEditor() const override;
    juce::AudioProcessorEditor* createEditor() override;
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

	bool isArpeggiatorEnabled() { return arpIsEnabled; }
	bool setArpeggiatorEnabled(bool enabled) { arpIsEnabled = enabled; return arpIsEnabled; }

private:
	bool arpIsEnabled;
    std::vector<int> heldNotes;
    size_t lastNoteIndex = 0;
    double sampleRate = 44100.0;
    int arpeggioSpeed = 120; // BPM
    int samplesPerArpeggioStep = 0;
    int sampleCounter = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDIArpeggiatorProcessor)
};
