//
// Created by Jay Vaughan on 22.02.25.
//
// MIDIArpeggiatorEditor.h
#ifndef MIDI_ARPEGGIATOR_EDITOR_H
#define MIDI_ARPEGGIATOR_EDITOR_H

#include <juce_analytics/juce_analytics.h>
#include <juce_animation/juce_animation.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_box2d/juce_box2d.h>
#include <juce_core/juce_core.h>
#include <juce_cryptography/juce_cryptography.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_javascript/juce_javascript.h>
#include <juce_midi_ci/juce_midi_ci.h>
#include <juce_opengl/juce_opengl.h>
#include <juce_osc/juce_osc.h>
#include <juce_product_unlocking/juce_product_unlocking.h>
#include <juce_video/juce_video.h>

#include <array>

class MIDIArpeggiatorProcessor;

class MIDIArpeggiatorEditor : public juce::AudioProcessorEditor, public juce::MidiKeyboardStateListener {
public:
	MIDIArpeggiatorEditor(MIDIArpeggiatorProcessor& processor);
	~MIDIArpeggiatorEditor() override;

	void paint(juce::Graphics& g) override;
	void resized() override;
	void handleNoteOn(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
	void handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float) override;

private:
	MIDIArpeggiatorProcessor& audioProcessor;
	juce::MidiKeyboardState keyboardState;
	juce::MidiKeyboardComponent keyboardComponent;
	juce::TextButton arpToggleButton;
};

#endif // MIDI_ARPEGGIATOR_EDITOR_H
