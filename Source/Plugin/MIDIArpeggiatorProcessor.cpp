/***
Example MIDI Processing plugin with arpeggiation.

 Best Practices Followed:
	- Minimal Audio Processing: The plugin disables audio channels since it only processes MIDI.
	- Efficient MIDI Buffer Handling: A new MidiBuffer is used to store modified events before swapping.
	- Safe MIDI Event Processing: The loop iterates over events and modifies them appropriately.
	- Avoiding Unnecessary Allocation: MIDI messages are processed in-place without excessive copying.
	- Proper JUCE Plugin Lifecycle Handling: Implements necessary overrides while keeping unnecessary ones minimal.
***/


#include "MIDIArpeggiatorProcessor.h"
#include "MIDIArpeggiatorEditor.h"

MIDIArpeggiatorProcessor::MIDIArpeggiatorProcessor()
		: AudioProcessor(BusesProperties().withInput("MIDI Input", juce::AudioChannelSet::disabled(), true)
								 .withOutput("MIDI Output", juce::AudioChannelSet::disabled(), true)) {}

const juce::String MIDIArpeggiatorProcessor::getName() const { return "MIDI Arpeggiator"; }

bool MIDIArpeggiatorProcessor::acceptsMidi() const { return true; }

bool MIDIArpeggiatorProcessor::producesMidi() const { return true; }

double MIDIArpeggiatorProcessor::getTailLengthSeconds() const { return 0.0; }

int MIDIArpeggiatorProcessor::getNumPrograms() { return 1; }

int MIDIArpeggiatorProcessor::getCurrentProgram() { return 0; }

void MIDIArpeggiatorProcessor::setCurrentProgram(int) {}

const juce::String MIDIArpeggiatorProcessor::getProgramName(int) { return {}; }

void MIDIArpeggiatorProcessor::changeProgramName(int, const juce::String &) {}

void MIDIArpeggiatorProcessor::prepareToPlay(double newSampleRate, int) {
	sampleRate = newSampleRate;
	samplesPerArpeggioStep = static_cast<int>((60.0 / arpeggioSpeed) * sampleRate / 4);
}

void MIDIArpeggiatorProcessor::releaseResources() {}

void MIDIArpeggiatorProcessor::processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &midiMessages) {
	juce::MidiBuffer processedMIDI;
	for (const auto meta: midiMessages) {
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
			processedMIDI.addEvent(juce::MidiMessage::noteOn(1, heldNotes[lastNoteIndex], (juce::uint8) 100), 0);
		}
	}
	midiMessages.swapWith(processedMIDI);
}

/* Simple in/out example */
static void processBlockMIDIThrough(juce::AudioBuffer<float> &, juce::MidiBuffer &midiMessages) {
	juce::MidiBuffer processedMidi;

	for (
		const auto meta
			: midiMessages) {
		auto message = meta.getMessage();
		auto timeStamp = meta.samplePosition;

// Example: Transpose all note-on events by +12 semitones
		if (message.isNoteOn()) {
			message = juce::MidiMessage::noteOn(message.getChannel(), message.getNoteNumber() + 12,
												message.getVelocity());
		}
// Example: Filter out all note-off messages (not recommended in real-world scenarios)
		else if (message.isNoteOff()) {
			continue;
		}

		processedMidi.addEvent(message, timeStamp);
	}

	midiMessages.swapWith(processedMidi);
}

bool MIDIArpeggiatorProcessor::hasEditor() const { return false; }

juce::AudioProcessorEditor *MIDIArpeggiatorProcessor::createEditor() { return nullptr; }

void MIDIArpeggiatorProcessor::getStateInformation(juce::MemoryBlock &) {}

void MIDIArpeggiatorProcessor::setStateInformation(const void *, int) {}
