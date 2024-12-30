#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

class MainComponent : public juce::Component
{
public:
    MainComponent();

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    juce::Image headerImage;

    // Language group
    juce::GroupComponent languageGroup {"languageGroup", "Language"};
    juce::ToggleButton englishButton {"English"};
    juce::ToggleButton frenchButton {"French"};

    // Display group
    juce::GroupComponent displayGroup {"displayGroup", "Display"};
    juce::ToggleButton windowedModeButton {"Windowed mode"};
    juce::ToggleButton fullscreenModeButton {"Full-screen mode"};

    // Keyboard Layout group
    juce::GroupComponent keyboardGroup {"keyboardGroup", "Keyboard Layout"};
    juce::ToggleButton qwertyButton {"QWERTY"};
    juce::ToggleButton qwertzButton {"QWERTZ (German)"};
    juce::ToggleButton azertyButton {"AZERTY (French)"};

    // Filters group
    juce::GroupComponent filtersGroup {"filtersGroup", "Filters"};
    juce::ToggleButton noFilterButton {"No filter"};
    juce::ToggleButton scanlinesButton {"Simple scanlines"};
    juce::ToggleButton crtButton {"CRT simulation"};

    // Audio group
    juce::GroupComponent audioGroup {"audioGroup", "Audio"};
    juce::ToggleButton musicEnabledButton {"Music enabled"};
    juce::ToggleButton soundEffectsButton {"Sound effects enabled"};

    // Stepper options
    juce::ToggleButton minimizeAfterLaunchButton {"Minimize after launch"};
    juce::ToggleButton quitAfterSessionButton {"Quit after play session"};
    juce::ToggleButton rememberPositionsButton {"Remember positions"};

    juce::TextButton quitButton {"Quit"};

    // Launch button
    juce::TextButton launchButton {"Launch Game"};
};

