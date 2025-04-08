#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include <BinaryData.h>

#include "apcStepperMainProcessor.h"

//#include "apcStepperMainEditor.h"
//#include "ShiftToggleButton.h"
//#include "apcToggleButton.h"
//#include "ToggleIconButton.h"
static const int apcPARAMETER_V1 = 0x01;
class apcToggleParameterButton : public juce::TextButton,private juce::AudioProcessorParameter::Listener{
public:
	apcToggleParameterButton(std::string buttonName,int step, int track,juce::Colour initialColour, juce::Colour toggleColour, apcStepperMainProcessor& p)
			: juce::TextButton(buttonName), buttonName(buttonName),step(step),track(track),initialColour(initialColour),
			  toggleColour(toggleColour), processor(p), isToggled(false)
	{
		setClickingTogglesState(true); // Enables automatic toggling
		setColour(juce::TextButton::buttonColourId, initialColour);

		button_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
						processor.getParameters(),buttonName,*this);
		current_attachment = processor.getParameters().getParameter("c" + processor.addLeadingZeros(step)); // Example ID
		if (current_attachment)
			current_attachment->addListener(this);
		currentColor = Colours::lightskyblue;

	}

	void clicked() override {
		isToggled = getToggleState();

		int midiNote =mapRowColumnToNote(step,track); // Map row index to MIDI notes (C1 and up)
		// !J! confirms that we can access MIDI from here:
		//isToggled ? processor.midiOutput->sendMessageNow(juce::MidiMessage::noteOn(6, midiNote, (juce::uint8) 55))
		//: processor.midiOutput->sendMessageNow(juce::MidiMessage::noteOn(6, midiNote, (juce::uint8) 00));

	}

	void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {
		g.fillAll(  current_attachment ?  currentColor : isToggled ? toggleColour :  initialColour);

		auto drawable = juce::Drawable::createFromImageData(BinaryData::button_svg, BinaryData::button_svgSize);
		if (drawable)
			drawable->drawWithin(g, drawable->getBounds().toFloat(), juce::RectanglePlacement::fillDestination, 1.0f);
	}
	void resized() override {

	}
	int mapRowColumnToNote(int step, int track) {

		return (step + track*8);

	}
	void parameterValueChanged(int parameterIndex, float newValue) override
	{
		isCurrent = (newValue > 0.5f);
		repaint();
	}

	void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override
	{
		// No-op (empty implementation is fine)
	}

private:
	juce::Colour initialColour;
	juce::Colour toggleColour;
	juce::Colour currentColor;
	bool isToggled;
	bool isCurrent;
	juce::String buttonName;
	apcStepperMainProcessor& processor;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> button_attachment;
	juce::AudioProcessorParameter* current_attachment = nullptr;
	int step;
	int track;
};
