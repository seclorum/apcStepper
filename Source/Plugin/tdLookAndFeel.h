#pragma once
#include "Common.h"

class tdLookAndFeel : public juce::LookAndFeel_V4 {
public:
    tdLookAndFeel();
    ~tdLookAndFeel() override = default;

    // ToggleButton painting
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    // ComboBox painting
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox& box) override;

    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;
    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                           bool isSeparator, bool isActive, bool isHighlighted,
                           bool isTicked, bool hasSubMenu,
                           const juce::String& text, const juce::String& shortcutText,
                           const juce::Drawable* icon, const juce::Colour* textColour) override;

    juce::Font getComboBoxFont(juce::ComboBox& box) override;
    juce::Font getPopupMenuFont() override;

private:
    juce::Colour backgroundColour = juce::Colour(0.2f, 0.2f, 0.2f); // Dark grey
    juce::Colour accentColour = juce::Colour(0.8f, 0.3f, 0.3f);     // Reddish accent
    juce::Colour textColour = juce::Colours::white;
    juce::Colour highlightColour = juce::Colour(0.9f, 0.4f, 0.4f);  // Brighter red for hover
};