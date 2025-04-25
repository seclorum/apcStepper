//
// Created by Jay Vaughan on 25.04.25.
//

#ifndef TRACKDECK_STEPPERDIALOG_H
#define TRACKDECK_STEPPERDIALOG_H

// StepperDialog.h
// Main dialog for trackDeck with MIDI controls and plugin hosting
// Copyright (c) Raw Material Software Limited
// Licensed under the ISC license: http://www.isc.org/downloads/software-support-policy/isc-license

#pragma once

#include "MidiControls.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <tracktion_engine/tracktion_engine.h>

namespace trackDeck
{

//==============================================================================
    class StepperDialog : public juce::Component, public juce::Button::Listener
    {
    public:
        StepperDialog();
        ~StepperDialog() override;

        void paint(juce::Graphics& g) override;
        void resized() override;
        void buttonClicked(juce::Button* button) override;

    private:
        void initializePluginHosting();
        void loadSettings();
        void saveSettings();

        tracktion_engine::Engine engine{"trackDeck"};
        std::unique_ptr<juce::AudioPluginFormatManager> formatManager;
        MidiControls midiControls;
        juce::TextButton launchStopButton;
        juce::TextButton quitButton;
        juce::ApplicationProperties appProperties;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepperDialog)
    };

} // namespace trackDeck

#endif //TRACKDECK_STEPPERDIALOG_H
