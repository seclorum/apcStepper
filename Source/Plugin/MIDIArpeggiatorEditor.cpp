//
// Created by Jay Vaughan on 22.02.25.
//
#include <juce_gui_extra/juce_gui_extra.h>

#include "MIDIArpeggiatorEditor.h"
#include "MIDIArpeggiatorProcessor.h"

MIDIArpeggiatorEditor::MIDIArpeggiatorEditor(MIDIArpeggiatorProcessor& p)
		: AudioProcessorEditor(&p), audioProcessor(p), keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard) {

	setSize(400, 200);
	keyboardState.addListener(this);

	addAndMakeVisible(keyboardComponent);
	addAndMakeVisible(arpToggleButton);

	arpToggleButton.setButtonText("Enable Arp");
	arpToggleButton.onClick = [this]() {
		bool arpEnabled = !audioProcessor.isArpeggiatorEnabled();
		audioProcessor.setArpeggiatorEnabled(arpEnabled);
		arpToggleButton.setButtonText(arpEnabled ? "Disable Arp" : "Enable Arp");
	};
}

MIDIArpeggiatorEditor::~MIDIArpeggiatorEditor() {
	keyboardState.removeListener(this);
}

void MIDIArpeggiatorEditor::paint(juce::Graphics& g) {
	g.fillAll(juce::Colours::black);
}

void MIDIArpeggiatorEditor::resized() {
	arpToggleButton.setBounds(10, 10, getWidth() - 20, 30);
	keyboardComponent.setBounds(10, 50, getWidth() - 20, getHeight() - 60);
}

void MIDIArpeggiatorEditor::handleNoteOn(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) {
	keyboardState.noteOn(midiChannel, midiNoteNumber, velocity);
}

void MIDIArpeggiatorEditor::handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float) {
	keyboardState.noteOff(midiChannel, midiNoteNumber, 0);
}

juce::AudioProcessorEditor* MIDIArpeggiatorProcessor::createEditor() {
	return new MIDIArpeggiatorEditor(*this);
}


