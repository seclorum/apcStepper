//
// Created by Tom Peak Walcher on 21.02.25.
//

#include "apcStepperMainEditor.h"
#include "apcStepperMainProcessor.h"
#include "apcControlPanel.h"


apcStepperMainEditor::apcStepperMainEditor(apcStepperMainProcessor& p)
    : juce::AudioProcessorEditor(p), processor(p)
{

#ifdef MELATONIN
    inspector.setVisible(true);
    inspector.toggle(true);
#endif

	// Hidden Tempo Slider (For Attachment)
	tempoSlider.setSliderStyle(Slider::LinearHorizontal);
	tempoSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
	tempoSlider.setRange(20.0f, 300.0f, 1.0f); // Typical tempo range
	tempoSlider.setVisible(true);
	addAndMakeVisible(tempoSlider);

	// Editable Tempo Label
	tempoLabel.setEditable(true);
	tempoLabel.setJustificationType(Justification::centred);
	tempoLabel.setColour(Label::textColourId, Colours::white);
	tempoLabel.setColour(Label::backgroundColourId, Colours::black);
	tempoLabel.setText(juce::String(tempoSlider.getValue()), dontSendNotification);
	tempoLabel.onTextChange = [this] {
		float newTempo = tempoLabel.getText().getFloatValue();
		tempoSlider.setValue(newTempo, juce::sendNotification);
	};
	addAndMakeVisible(tempoLabel);

	tempoAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
			processor.getParameters(), "tempo", tempoSlider);

	// Sync tempo label with slider
	tempoSlider.onValueChange = [this] { syncTempo(); };

	// Create TabbedComponent safely
    tabbedComponent = std::make_unique<TabbedComponent>(TabbedButtonBar::TabsAtTop);
    jassert(tabbedComponent != nullptr);


    if (tabbedComponent)
    {
        // Use smart pointer to ensure safe memory management
        tabbedComponent->addTab("apcStepper", juce::Colours::darkgrey, new apcControlPanel(processor), true);
        tabbedComponent->addTab("About", juce::Colours::lightblue, new apcAboutBox(), true);
        addAndMakeVisible(tabbedComponent.get());
    }

    setSize(800, 600); // Ensure editor has a set size
    setResizable(true, true );

}

// Syncs the tempo label with the hidden slider
void apcStepperMainEditor::syncTempo()
{
	tempoLabel.setText(juce::String(tempoSlider.getValue()), juce::dontSendNotification);
	processor.setTempo(roundToInt(tempoSlider.getValue()));
}

apcStepperMainEditor::~apcStepperMainEditor() = default;

void apcStepperMainEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::green); // Ensure background is white
}

void apcStepperMainEditor::resized()
{
	auto bounds = getLocalBounds();
	auto sliderHeight = 40;

	tempoSlider.setBounds(bounds.removeFromTop(40));
	tempoLabel.setBounds(bounds.removeFromTop(40).reduced(10)); // Centered editable label

	if (tabbedComponent)
        tabbedComponent->setBounds(bounds);
}
