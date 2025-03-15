//
// Created by Tom Peak Walcher on 21.02.25.
//

#include "apcStepperMainEditor.h"
#include "apcStepperMainProcessor.h"
#include "apcControlPanel.h"
#include "apcLog.h"



apcStepperMainEditor::apcStepperMainEditor(apcStepperMainProcessor& p)
    : juce::AudioProcessorEditor(p), processor(p)
{

#ifdef MELATONIN
    inspector.setVisible(true);
    inspector.toggle(true);
#endif

	// Hidden Tempo Slider (For Attachment)
		// Create TabbedComponent safely
    tabbedComponent = std::make_unique<TabbedComponent>(TabbedButtonBar::TabsAtTop);
    jassert(tabbedComponent != nullptr);


    if (tabbedComponent)
    {
        // Use smart pointer to ensure safe memory management
        tabbedComponent->addTab("apcStepper", juce::Colours::darkgrey, new apcControlPanel(processor), true);
        tabbedComponent->addTab("About", juce::Colours::lightblue, new apcAboutBox(), true);
        tabbedComponent->addTab("Log", juce::Colours::lightblue, new apcLog(), true);
        addAndMakeVisible(tabbedComponent.get());
    }

    setSize(800, 600); // Ensure editor has a set size
	setResizeLimits(540, 420, 1200, 900);

	setResizable(true, true );

}

// Syncs the tempo label with the hidden slider

apcStepperMainEditor::~apcStepperMainEditor() = default;

void apcStepperMainEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::green); // Ensure background is white
}

void apcStepperMainEditor::resized()
{
	auto bounds = getLocalBounds();


	if (tabbedComponent)
        tabbedComponent->setBounds(bounds);
}
