#include "MainComponent.h"

MainComponent::MainComponent()
{
    // Add UI components
    addAndMakeVisible(languageGroup);
    addAndMakeVisible(englishButton);
    addAndMakeVisible(frenchButton);

    addAndMakeVisible(displayGroup);
    addAndMakeVisible(windowedModeButton);
    addAndMakeVisible(fullscreenModeButton);

    addAndMakeVisible(keyboardGroup);
    addAndMakeVisible(qwertyButton);
    addAndMakeVisible(qwertzButton);
    addAndMakeVisible(azertyButton);

    addAndMakeVisible(filtersGroup);
    addAndMakeVisible(noFilterButton);
    addAndMakeVisible(scanlinesButton);
    addAndMakeVisible(crtButton);

    addAndMakeVisible(audioGroup);
    addAndMakeVisible(musicEnabledButton);
    addAndMakeVisible(soundEffectsButton);

    addAndMakeVisible(minimizeAfterLaunchButton);
    addAndMakeVisible(quitAfterSessionButton);
    addAndMakeVisible(rememberPositionsButton);

    addAndMakeVisible(quitButton);
    addAndMakeVisible(launchButton);

    // Set radio button groups
    englishButton.setRadioGroupId(1);
    frenchButton.setRadioGroupId(1);

    windowedModeButton.setRadioGroupId(2);
    fullscreenModeButton.setRadioGroupId(2);

    qwertyButton.setRadioGroupId(3);
    qwertzButton.setRadioGroupId(3);
    azertyButton.setRadioGroupId(3);

    noFilterButton.setRadioGroupId(4);
    scanlinesButton.setRadioGroupId(4);
    crtButton.setRadioGroupId(4);

    setSize(800, 600);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromRGB(45, 55, 65)); // Set background color similar to the screenshot
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    // Header space
    auto headerArea = bounds.removeFromTop(50);

    // Column layout
    auto columnWidth = bounds.getWidth() / 3;

    auto leftColumn = bounds.removeFromLeft(columnWidth).reduced(5);
    auto middleColumn = bounds.removeFromLeft(columnWidth).reduced(5);
    auto rightColumn = bounds.reduced(5);

    // Language group
    languageGroup.setBounds(leftColumn.removeFromTop(100));
    englishButton.setBounds(languageGroup.getBounds().reduced(15).removeFromTop(20));
    frenchButton.setBounds(languageGroup.getBounds().reduced(15).removeFromTop(40));

    // Display group
    displayGroup.setBounds(leftColumn.removeFromTop(100));
    windowedModeButton.setBounds(displayGroup.getBounds().reduced(15).removeFromTop(20));
    fullscreenModeButton.setBounds(displayGroup.getBounds().reduced(15).removeFromTop(40));

    // Keyboard Layout group
    keyboardGroup.setBounds(middleColumn.removeFromTop(100));
    qwertyButton.setBounds(keyboardGroup.getBounds().reduced(15).removeFromTop(20));
    qwertzButton.setBounds(keyboardGroup.getBounds().reduced(15).removeFromTop(40));
    azertyButton.setBounds(keyboardGroup.getBounds().reduced(15).removeFromTop(60));

    // Filters group
    filtersGroup.setBounds(middleColumn.removeFromTop(100));
    noFilterButton.setBounds(filtersGroup.getBounds().reduced(15).removeFromTop(20));
    scanlinesButton.setBounds(filtersGroup.getBounds().reduced(15).removeFromTop(40));
    crtButton.setBounds(filtersGroup.getBounds().reduced(15).removeFromTop(60));

    // Audio group
    audioGroup.setBounds(rightColumn.removeFromTop(80));
    musicEnabledButton.setBounds(audioGroup.getBounds().reduced(15).removeFromTop(20));
    soundEffectsButton.setBounds(audioGroup.getBounds().reduced(15).removeFromTop(40));

    minimizeAfterLaunchButton.setBounds(rightColumn.removeFromTop(30).reduced(5));
    quitAfterSessionButton.setBounds(rightColumn.removeFromTop(30).reduced(5));
    rememberPositionsButton.setBounds(rightColumn.removeFromTop(30).reduced(5));

    quitButton.setBounds(rightColumn.removeFromTop(40).reduced(5));
    launchButton.setBounds(bounds.removeFromBottom(50).reduced(5));
}

