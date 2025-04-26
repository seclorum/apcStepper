//
// Created by Jay Vaughan on 25.04.25.
//

#include "StepperApplication.h"
#include "StepperDialog.h"

// StepperApplication.cpp
// Application entry point implementation for trackDeck
// Copyright (c) Raw Material Software Limited
// Licensed under the ISC license: http://www.isc.org/downloads/software-support-policy/isc-license

#include "StepperApplication.h"
#include "StepperDialog.h"

namespace trackDeck
{

//==============================================================================
    const juce::String StepperApplication::getApplicationName() { return "trackDeck"; }
    const juce::String StepperApplication::getApplicationVersion() { return "1.0.0"; }

    void StepperApplication::initialise(const juce::String&)
    {
        engine = std::make_unique<tracktion::Engine>(getApplicationName());
        auto dialog = std::make_unique<StepperDialog>(*engine);
        mainWindow = std::make_unique<MainWindow>("trackDeck",  dialog.get(), *this);
    }

    void StepperApplication::shutdown()
    {
        mainWindow.reset();
    }

//==============================================================================
    StepperApplication::MainWindow::MainWindow(const juce::String& name, juce::Component* component, JUCEApplication& app)
            : DocumentWindow(name, juce::Colours::lightgrey, allButtons), appRef(app)
    {
        setUsingNativeTitleBar(true);
        setContentOwned(component, true);
        centreWithSize(component->getWidth(), component->getHeight());
        setVisible(true);
    }

    void StepperApplication::MainWindow::closeButtonPressed()
    {
        appRef.systemRequestedQuit();
    }

} // namespace trackDeck

//==============================================================================
START_JUCE_APPLICATION(trackDeck::StepperApplication);