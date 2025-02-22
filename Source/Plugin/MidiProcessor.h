#include <juce_audio_processors/juce_audio_processors.h>

class MidiProcessor : public juce::AudioProcessor {
public:
    MidiProcessor()
        : AudioProcessor(BusesProperties().withInput("MIDI Input", juce::AudioChannelSet::disabled(), true)
                                        .withOutput("MIDI Output", juce::AudioChannelSet::disabled(), true)) {}
    
    const juce::String getName() const override { return "MIDI Processor"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    
    void prepareToPlay(double sampleRate, int samplesPerBlock) override {
        // Typically, MIDI-only plugins don't need to do much here.
    }
    
    void releaseResources() override {}
    
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer& midiMessages) override {
        juce::MidiBuffer processedMidi;
        
        for (const auto meta : midiMessages) {
            auto message = meta.getMessage();
            auto timeStamp = meta.samplePosition;
            
            // Example: Transpose all note-on events by +12 semitones
            if (message.isNoteOn()) {
                message = juce::MidiMessage::noteOn(message.getChannel(), message.getNoteNumber() + 12, message.getVelocity());
            }
            // Example: Filter out all note-off messages (not recommended in real-world scenarios)
            else if (message.isNoteOff()) {
                continue;
            }
            
            processedMidi.addEvent(message, timeStamp);
        }
        
        midiMessages.swapWith(processedMidi);
    }
    
    bool hasEditor() const override { return false; }
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiProcessor)
};

//juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
//    return new MidiProcessor();
//}
