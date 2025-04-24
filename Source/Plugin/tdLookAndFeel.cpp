//
// Created by Tom Peak Walcher on 24.04.25.
//

#include "tdLookAndFeel.h"

tdLookAndFeel::tdLookAndFeel() {
    setColour(juce::ToggleButton::textColourId, textColour);
    setColour(juce::ComboBox::backgroundColourId, backgroundColour);
    setColour(juce::ComboBox::textColourId, textColour);
    setColour(juce::ComboBox::outlineColourId, accentColour);
    setColour(juce::PopupMenu::backgroundColourId, backgroundColour);
    setColour(juce::PopupMenu::textColourId, textColour);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, highlightColour);
    setColour(juce::PopupMenu::highlightedTextColourId, textColour);
}

void tdLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) {
    auto bounds = button.getLocalBounds().toFloat().reduced(2.0f);
    auto cornerSize = 6.0f;

    // Background
    g.setColour(button.getToggleState() ? accentColour : backgroundColour);
    g.fillRoundedRectangle(bounds, cornerSize);

    // Border
    g.setColour(shouldDrawButtonAsHighlighted ? highlightColour : accentColour);
    g.drawRoundedRectangle(bounds, cornerSize, 1.0f);

    // Text
    g.setColour(textColour);
    g.setFont(Font(14.0f));
    g.drawText(button.getButtonText(), bounds, juce::Justification::centred, true);
}

void tdLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                     int buttonX, int buttonY, int buttonW, int buttonH,
                                     juce::ComboBox& box) {
    auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat().reduced(2.0f);
    auto cornerSize = 4.0f;

    // Background
    g.setColour(backgroundColour);
    g.fillRoundedRectangle(bounds, cornerSize);

    // Border
    g.setColour(box.isMouseOver() ? highlightColour : accentColour);
    g.drawRoundedRectangle(bounds, cornerSize, 1.0f);


    // Text
    auto textBounds = bounds.reduced(5, 0);
    g.setColour(textColour);
    g.setFont(getComboBoxFont(box));
    g.drawText(box.getText(), textBounds, juce::Justification::centredLeft, true);
}

void tdLookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height) {
    g.fillAll(backgroundColour);
    g.setColour(accentColour);
    g.drawRect(0, 0, width, height, 1);
}

void tdLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                                          bool isSeparator, bool isActive, bool isHighlighted,
                                          bool isTicked, bool hasSubMenu,
                                          const juce::String& text, const juce::String& shortcutText,
                                          const juce::Drawable* icon, const juce::Colour* textColourToUse) {
    if (isSeparator) {
        g.setColour(accentColour);
        g.drawLine(area.getX() + 10, area.getCentreY(), area.getRight() - 10, area.getCentreY(), 1.0f);
        return;
    }

    auto bounds = area.reduced(2);
    if (isHighlighted && isActive) {
        g.setColour(highlightColour);
        g.fillRect(bounds);
        g.setColour(accentColour);
        g.drawRect(bounds, 1);
    }

    g.setColour(isTicked ? accentColour : textColour);
    g.setFont(getPopupMenuFont());
    g.drawText(text, bounds.withTrimmedLeft(10), juce::Justification::centredLeft, true);

    if (hasSubMenu) {
        auto arrowBounds = bounds.removeFromRight(20);
        juce::Path arrow;
        arrow.startNewSubPath(arrowBounds.getCentreX() - 3, arrowBounds.getCentreY() - 5);
        arrow.lineTo(arrowBounds.getCentreX() + 3, arrowBounds.getCentreY());
        arrow.lineTo(arrowBounds.getCentreX() - 3, arrowBounds.getCentreY() + 5);
        g.fillPath(arrow);
    }
}

juce::Font tdLookAndFeel::getComboBoxFont(juce::ComboBox& /*box*/) {
    return juce::Font(14.0f);
}

juce::Font tdLookAndFeel::getPopupMenuFont() {
    return juce::Font(14.0f);
}
