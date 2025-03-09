#include "ToggleSquareShift.h"
#include "ShiftToggleSquareButton.h"
#include <BinaryData.h>

ToggleSquareShift::ToggleSquareShift() {
    for (int i = 0; i < rows; ++i) {
        rowButtons.add(std::make_unique<ShiftToggleSquareButton>(juce::Colours::grey, juce::Colours::blue, juce::Colours::orange, juce::Image()));
        addAndMakeVisible(rowButtons.getLast());
        rowButtons[i]->onClick = [this, i]() { if (!this->shiftMode) squareClicked(i); };
        rightPanel.items.add(juce::FlexItem(*rowButtons.getLast()).withFlex(1).withMargin(6));
    }

    playToggleButton = std::make_unique<ToggleSquare>(juce::Colours::green, juce::Colours::darkgreen, BinaryData::playcircle_svg, BinaryData::playcircle_svgSize);
    addAndMakeVisible(playToggleButton.get());

    stopToggleButton = std::make_unique<ToggleSquare>(juce::Colours::red, juce::Colours::darkgrey, BinaryData::stopcircle_svg, BinaryData::stopcircle_svgSize);
    addAndMakeVisible(stopToggleButton.get());

    shiftToggleButton = std::make_unique<ToggleSquare>(juce::Colours::blue, juce::Colours::orange, BinaryData::shift_svg, BinaryData::shift_svgSize);
    addAndMakeVisible(shiftToggleButton.get());

    shiftToggleButton->onClick = [this]() {
        shiftMode = !shiftMode;
        updateRowButtonColors();
    };

    downButtons.items.add(juce::FlexItem(*playToggleButton).withFlex(1));
    downButtons.items.add(juce::FlexItem(*stopToggleButton).withFlex(1));
    downButtons.items.add(juce::FlexItem(*shiftToggleButton).withFlex(1));

    addAndMakeVisible(playToggleButton.get());
    addAndMakeVisible(stopToggleButton.get());
    addAndMakeVisible(shiftToggleButton.get());

    for (auto& button : rowButtons) {
        addAndMakeVisible(button);
    };
    rightPanelContainer.items.add(juce::FlexItem(rightPanel).withFlex(4));
    rightPanelContainer.items.add(juce::FlexItem(downButtons).withFlex(2));
    rightPanelContainer.performLayout(getLocalBounds().toFloat());
}

void ToggleSquareShift::resized() {
    auto bounds = getLocalBounds();

    rightPanel.flexDirection = juce::FlexBox::Direction::column;
    rightPanel.justifyContent = juce::FlexBox::JustifyContent::center;
    rightPanel.alignItems = juce::FlexBox::AlignItems::stretch;

    rightPanelContainer.flexDirection = juce::FlexBox::Direction::column;
    rightPanelContainer.justifyContent = juce::FlexBox::JustifyContent::center;
    rightPanelContainer.alignItems = juce::FlexBox::AlignItems::stretch;

    downButtons.flexDirection = juce::FlexBox::Direction::column;
    downButtons.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    downButtons.alignItems = juce::FlexBox::AlignItems::flexStart;

    downButtons.performLayout(bounds.toFloat());
    rightPanelContainer.performLayout(bounds.toFloat());


    int squareSize = playToggleButton->getHeight();

    playToggleButton->setSize(squareSize, squareSize);
    stopToggleButton->setSize(squareSize, squareSize);
    shiftToggleButton->setSize(squareSize, squareSize);

    playToggleButton->setBounds(playToggleButton->getX() + (bounds.getWidth() /2)  - (squareSize / 2), playToggleButton->getY()  , squareSize, squareSize);
        stopToggleButton->setBounds(stopToggleButton->getX() + (bounds.getWidth() / 2)  - (squareSize / 2), stopToggleButton->getY(), squareSize, squareSize);
    shiftToggleButton->setBounds(shiftToggleButton->getX() + (bounds.getWidth() /2) - (squareSize / 2), shiftToggleButton->getY(), squareSize, squareSize);
}

