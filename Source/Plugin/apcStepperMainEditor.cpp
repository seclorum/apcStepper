//
// Created by Tom Peak Walcher on 21.02.25.
//

#include "apcStepperMainEditor.h"
#include "apcStepperMainProcessor.h"
#include "apcStepperGrid.h"


apcStepperMainEditor::apcStepperMainEditor(apcStepperMainProcessor& p)
    : juce::AudioProcessorEditor(p), processor(p)
{

    inspector.setVisible(true);
    inspector.toggle(true);

    // Create TabbedComponent safely
    tabbedComponent = std::make_unique<TabbedComponent>(TabbedButtonBar::TabsAtTop);
    jassert(tabbedComponent != nullptr);


    if (tabbedComponent)
    {
        // Use smart pointer to ensure safe memory management
        tabbedComponent->addTab("apcStepper", juce::Colours::blue, new apcStepperGrid(processor), true);
        tabbedComponent->addTab("About", juce::Colours::lightblue, new apcAboutBox(), true);
        addAndMakeVisible(tabbedComponent.get());
    }

    setSize(800, 600); // Ensure editor has a set size
    setResizable(true, true );

}

apcStepperMainEditor::~apcStepperMainEditor() = default;

void apcStepperMainEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::green); // Ensure background is white
}

void apcStepperMainEditor::resized()
{
    if (tabbedComponent)
        tabbedComponent->setBounds(getLocalBounds()); // Use getLocalBounds() instead of getBounds()
}
