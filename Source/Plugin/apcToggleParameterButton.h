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
class apcToggleParameterButton : public juce::TextButton {
public:
	apcToggleParameterButton(std::string buttonName, juce::Colour initialColour, juce::Colour toggleColour, apcStepperMainProcessor& p)
			: juce::TextButton(buttonName), buttonName(buttonName), initialColour(initialColour),
			  toggleColour(toggleColour), processor(p), isToggled(false)
	{
		setClickingTogglesState(true); // Enables automatic toggling
		setColour(juce::TextButton::buttonColourId, initialColour);



		button_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
						processor.getParameters(),buttonName,*this);

	}

	void clicked() override {
		isToggled = getToggleState();
		//processor.parameterChanged(buttonName,isToggled);

	}

	void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {
		g.fillAll(isToggled ? toggleColour : initialColour);

		auto drawable = juce::Drawable::createFromImageData(BinaryData::button_svg, BinaryData::button_svgSize);
		if (drawable)
			drawable->drawWithin(g, drawable->getBounds().toFloat(), juce::RectanglePlacement::fillDestination, 1.0f);
	}
	void resized() override {

	}


private:
	juce::Colour initialColour;
	juce::Colour toggleColour;
	bool isToggled;
	juce::String buttonName;
	apcStepperMainProcessor& processor;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> button_attachment;
};
