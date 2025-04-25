//
// Created by Jay Vaughan on 25.04.25.
//

// StepperDialog.cpp
// Main dialog implementation for trackDeck
// Copyright (c) Raw Material Software Limited
// Licensed under the ISC license: http://www.isc.org/downloads/software-support-policy/isc-license

#include "StepperDialog.h"

namespace trackDeck
{

//==============================================================================
    StepperDialog::StepperDialog()
            : midiControls(engine)
    {
        formatManager = std::make_unique<juce::AudioPluginFormatManager>();
        initializePluginHosting();

        addAndMakeVisible(midiControls);
        addAndMakeVisible(launchStopButton);
        addAndMakeVisible(quitButton);

        launchStopButton.setButtonText("Launch/Stop");
        quitButton.setButtonText("Quit");

        launchStopButton.addListener(this);
        quitButton.addListener(this);

        loadSettings();
        setSize(720, 800);
    }

    StepperDialog::~StepperDialog()
    {
        launchStopButton.removeListener(this);
        quitButton.removeListener(this);
        saveSettings();
    }

    void StepperDialog::paint(juce::Graphics& g)
    {
        g.fillAll(juce::Colours::white);
    }

    void StepperDialog::resized()
    {
        auto area = getLocalBounds().reduced(10);
        midiControls.setBounds(area.removeFromTop(700));
        launchStopButton.setBounds(area.removeFromTop(40));
        quitButton.setBounds(area.removeFromTop(40).withTrimmedTop(10));
    }

    void StepperDialog::buttonClicked(juce::Button* button)
    {
        if (button == &launchStopButton)
        {
            // TODO: Implement launch/stop functionality (e.g., toggle plugin processing)
        }
        else if (button == &quitButton)
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    }

    void StepperDialog::initializePluginHosting()
    {
        formatManager->addDefaultFormats(); // Adds AU, VST3, etc.

        const juce::String pluginId = "io.enabled.plugins.trackDeck"; // Match BUNDLE_ID
        auto* pluginList = formatManager->getKnownPluginList();
        auto pluginTypes = pluginList->getTypesForIdentifier(pluginId);

        if (!pluginTypes.isEmpty())
        {
            formatManager->createPluginInstanceAsync(
                    pluginTypes.getFirst(),
                    44100.0, 512,
                    [](std::unique_ptr<juce::AudioPluginInstance> instance, const juce::String& error)
                    {
                        if (instance)
                            juce::Logger::writeToLog("Plugin loaded successfully: " + instance->getName());
                        else
                            juce::Logger::writeToLog("Failed to load plugin: " + error);
                    });
        }

        if (auto plugin = engine.getPluginManager().createPlugin(pluginId))
            juce::Logger::writeToLog("Tracktion plugin loaded successfully: " + plugin->getName());
    }

    void StepperDialog::loadSettings()
    {
        juce::PropertiesFile::Options options;
        options.applicationName = "trackDeck";
        options.commonToAllUsers = false;
        options.filenameSuffix = ".props";
        options.storageFormat = juce::PropertiesFile::storeAsXML;
        options.osxLibrarySubFolder = "Application Support";

        appProperties.setStorageParameters(options);

        if (auto lastBounds = appProperties.getUserSettings()->getValue("lastWindowBounds"); !lastBounds.isEmpty())
            setBounds(juce::Rectangle<int>::fromString(lastBounds));
        else
            centreWithSize(720, 800);
    }

    void StepperDialog::saveSettings()
    {
        appProperties.getUserSettings()->setValue("lastWindowBounds", getBounds().toString());
        appProperties.saveIfNeeded();
        appProperties.closeFiles();
    }

} // namespace trackDeck