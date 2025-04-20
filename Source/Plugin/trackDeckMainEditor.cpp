//
// Created by Tom Peak Walcher on 21.02.25.
//

#include "trackDeckMainEditor.h"
#include "trackDeckMainProcessor.h"
#include "apcControlPanel.h"
#include "apcLog.h"

#ifdef USE_MELATONIN_INSPECTOR
#include <melatonin_inspector/melatonin_inspector.h>
#endif


trackDeckMainEditor::trackDeckMainEditor(trackDeckMainProcessor& p)
    : juce::AudioProcessorEditor(p), processor(p)
{

#ifdef USE_MELATONIN_INSPECTOR
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
        tabbedComponent->addTab("trackDeck", juce::Colour(0xff041937), new apcControlPanel(processor), true);
        tabbedComponent->addTab("About", juce::Colours::lightblue, new apcAboutBox(), true);
        tabbedComponent->addTab("Log", juce::Colours::lightblue, new apcLog(), true);
        addAndMakeVisible(tabbedComponent.get());
    }

    setSize(1240, 800); // Ensure editor has a set size
	setResizeLimits(540, 420, 1200, 900);

	setResizable(true, true );

}

// Syncs the tempo label with the hidden slider

trackDeckMainEditor::~trackDeckMainEditor() = default;

void trackDeckMainEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff0D47A1)); // Ensure background is white

}

void trackDeckMainEditor::resized()
{
	auto bounds = getLocalBounds();


	if (tabbedComponent)
        tabbedComponent->setBounds(bounds);
}
