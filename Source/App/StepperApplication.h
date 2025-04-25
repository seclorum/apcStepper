//
// Created by Jay Vaughan on 25.04.25.
//

#ifndef TRACKDECK_STEPPERAPPLICATION_H
#define TRACKDECK_STEPPERAPPLICATION_H

// StepperApplication.h
// Application entry point for trackDeck
// Copyright (c) Raw Material Software Limited
// Licensed under the ISC license: http://www.isc.org/downloads/software-support-policy/isc-license

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace trackDeck
{

//==============================================================================
    class StepperApplication : public juce::JUCEApplication
    {
    public:
        const juce::String getApplicationName() override;
        const juce::String getApplicationVersion() override;
        void initialise(const juce::String& commandLine) override;
        void shutdown() override;

    private:
        class MainWindow : public juce::DocumentWindow
        {
        public:
            MainWindow(const juce::String& name, juce::Component* component, JUCEApplication& app);
            void closeButtonPressed() override;

        private:
            JUCEApplication& appRef;
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
        };

        std::unique_ptr<MainWindow> mainWindow;
    };

} // namespace trackDeck

#endif //TRACKDECK_STEPPERAPPLICATION_H
