// StepperDialog.cpp - Main Implementation
#include "StepperDialog.h"

#include "juce_audio_processors/format_types/juce_VST3Common.h"

namespace trackDeck {
    StepperDialog::StepperDialog(tracktion::Engine& e)
        : engine(e),
          midiControls(std::make_unique<MidiControls>(engine))
    {
        // Initialize systems
        initializeEdit();
        initializeAudio();
        initializePlugin();
        loadSettings();

        // Setup UI
        addAndMakeVisible(midiControls.get());
        addAndMakeVisible(launchStopButton);
        addAndMakeVisible(quitButton);
        addAndMakeVisible(trackTabs);

        // Configure buttons
        launchStopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred);
        quitButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
        launchStopButton.addListener(this);
        quitButton.addListener(this);

        // Initialize tracks
        for (int i = 0; i < 8; ++i) { // 8 tracks
            auto trackComp = std::make_unique<tdTrack>(*this, i);
            trackTabs.addTab("Track " + juce::String(i+1), juce::Colours::darkgrey, trackComp.get(), false);
            tracks.push_back(std::move(trackComp));
        }

        // Add controller bar
        trackController = std::make_unique<tdTrackControllerBar>(*this, 0);
        addAndMakeVisible(trackController.get());

        setSize(800, 600);
    }

    StepperDialog::~StepperDialog() {
        saveSettings();
        if (playbackContext) {
            playbackContext->stop();
            playbackContext.reset();
        }
    }

    void StepperDialog::initializeEdit() {
        edit = std::make_unique<tracktion::Edit>(engine, tracktion::Edit::EditRole::forEditing, nullptr);
        
        // Create initial MIDI track and clip
        auto track = edit->insertNewAudioTrack(tracktion::TrackInsertPoint(nullptr, nullptr), nullptr);
        if (auto track = edit->insertNewAudioTrack(tracktion::TrackInsertPoint(nullptr, nullptr), nullptr))
        {
            // Create MIDI clip with proper time range
            auto clip = track->insertMIDIClip(juce::toString(tracktion::TimePosition::fromSeconds(0.0)), juce::toString(tracktion::TimePosition::fromSeconds(0.0)));,
                                              tracktion::TimePosition::fromSeconds(4.0),
                                              nullptr);

            if (clip != nullptr)
            {
                currentClip = dynamic_cast<tracktion::MidiClip*>(clip);

                if (currentClip != nullptr)
                {
                    // Set up 4-bar loop (16 beats at 4/4)
                    currentClip->setLoopRangeBeats(tracktion::BeatRange(
                        tracktion::BeatPosition::fromBeats(0.0),
                        tracktion::BeatPosition::fromBeats(16.0)
                    ));
                    //currentClip->setLooped(true);

                    // Get sequence and add notes
                    auto& sequence = currentClip->getSequence();

                    // Clear any existing notes first
                    sequence.clear(nullptr);

                    // Add 16th note pattern
                    for (int i = 0; i < 16; ++i)
                    {
                        sequence.addNote(
                            60,                         // Pitch (middle C)
                            tracktion::BeatPosition::fromBeats(i * 0.25), // Start (16th notes)
                            tracktion::BeatDuration::fromBeats(0.2),      // Length
                            100,                        // Velocity
                            0,                          // Colour index
                            nullptr                     // Undo manager
                        );
                    }

                    // Ensure clip length matches loop length
                    currentClip->setLength(
                        edit->tempoSequence.toTime(toString(tracktion::BeatPosition::fromBeats(16.0))),
                        tracktion::TimePosition()
                    );
                }
            }
        }
        
        audioTransport = &edit->getTransport();
    }

    void StepperDialog::initializePlugin() {
        if (!edit) return;
        
        const juce::String pluginID = "io.enabled.plugins.trackDeck";
        if (auto track = getFirstAudioTrack(*edit)) {
            if (auto plugin = edit->getPluginCache().createNewPlugin(pluginID, {})) {
                track->pluginList.insertPlugin(*plugin, 0, nullptr);
                currentPlugin = plugin;
                currentPlugin->state.addListener(this);
                
                // Connect plugin parameters to our UI
                syncPluginWithClip();
            }
        }
    }

    void StepperDialog::syncPluginWithClip() {
        if (!currentClip || !currentPlugin) return;
        
        // Get the plugin's parameter list
        auto params = currentPlugin->getAutomatableParameters();
        
        // Map clip properties to plugin parameters
        if (params.size() > 0) {
            // Example: Sync loop length
            params[0]->setParameter(currentClip->getLoopLengthBeats().inBeats() / 16.0, 
                                  sendNotification);
        }
    }

    void StepperDialog::updateClipLooping() {
        if (!currentClip) return;
        
        // Get current loop state from UI or plugin
        bool shouldLoop = true; // Default or get from UI

        currentClip->setLooped(shouldLoop);
        
        // Update loop range based on step sequencer
        auto loopLength = tracktion::BeatDuration(16.0); // 4 bars
        currentClip->setLoopRangeBeats({0.0, loopLength.inBeats()});
        
        // Notify plugin of changes
        syncPluginWithClip();
    }

    void StepperDialog::buttonClicked(juce::Button* button) {
        if (button == &launchStopButton) {
            togglePlayback();
        }
        else if (button == &quitButton) {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    }

    void StepperDialog::resized() {
        auto bounds = getLocalBounds().reduced(10);
        
        // Top area - MIDI controls
        midiControls->setBounds(bounds.removeFromTop(100));
        
        // Middle area - Track tabs and controller
        trackTabs.setBounds(bounds.removeFromTop(300));
        trackController->setBounds(bounds.removeFromTop(100));
        
        // Bottom area - buttons
        auto buttonArea = bounds.removeFromBottom(40);
        launchStopButton.setBounds(buttonArea.removeFromLeft(150));
        quitButton.setBounds(buttonArea.removeFromLeft(150));
    }

    // ValueTree callbacks for plugin parameter changes
    void StepperDialog::valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property) {
        if (currentPlugin && tree == currentPlugin->state) {
            // Handle parameter changes from plugin
            DBG("Plugin parameter changed: " << property.toString());
            
            // Update clip if needed
            if (property == juce::Identifier("loopLength")) {
                if (currentClip) {
                    auto loopBeats = tree.getProperty(property) * 16.0;
                    currentClip->setLoopRangeBeats({0.0, loopBeats});
                }
            }
        }
    }
}
