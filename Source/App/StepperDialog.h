// StepperDialog.h - Updated Header
#pragma once

#include "MidiControls.h"
#include "../Plugin/tdTrack.h"
#include "../Plugin/tdTrackControllerBar.h"



namespace trackDeck {
    class StepperDialog : public juce::Component,
                         public juce::Button::Listener,
                         public juce::ChangeListener,
                         public tracktion::ValueTreeAllEventListener
    {
    public:
        StepperDialog(tracktion::Engine& e);
        ~StepperDialog() override;
        
        void paint(juce::Graphics&) override;
        void resized() override;
        void buttonClicked(juce::Button*) override;
        
        // ValueTree listener callbacks
        void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override;
        void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override;
        void valueTreeChildRemoved(juce::ValueTree&, juce::ValueTree&, int) override;
        
    private:
        void initializeEdit();
        void initializeAudio();
        void initializePlugin();
        void loadSettings();
        void saveSettings();
        
        void createNewClip();
        void togglePlayback();
        void updateClipLooping();
        void syncPluginWithClip();
        
        tracktion::Engine& engine;
        std::unique_ptr<tracktion::Edit> edit;
        std::unique_ptr<MidiControls> midiControls;
        
        // UI Components
        juce::TextButton launchStopButton {"Launch/Stop"};
        juce::TextButton quitButton {"Quit"};
        juce::TabbedComponent trackTabs {juce::TabbedButtonBar::TabsAtTop};
        
        // Clip and plugin management
        tracktion::MidiClip* currentClip = nullptr;
        tracktion::Plugin* currentPlugin = nullptr;
        
        // Track components
        std::vector<std::unique_ptr<tdTrack>> tracks;
        std::unique_ptr<tdTrackControllerBar> trackController;
        
        // Audio transport
        std::unique_ptr<tracktion::EditPlaybackContext> playbackContext;
        tracktion::TransportControl* audioTransport = nullptr;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepperDialog)
    };
}
