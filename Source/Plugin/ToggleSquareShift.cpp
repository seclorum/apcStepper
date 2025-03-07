#include "ToggleSquareShift.h"

ToggleSquareShift::ToggleSquareShift(juce::OwnedArray<ToggleSquare>& rowButtons,
                                    std::unique_ptr<ToggleSquare>& playToggleButton,
                                    std::unique_ptr<ToggleSquare>& stopToggleButton,
                                    std::unique_ptr<ToggleSquare>& shiftToggleButton)
    : rowButtons(rowButtons),
      playToggleButton(playToggleButton),
      stopToggleButton(stopToggleButton),
      shiftToggleButton(shiftToggleButton) {

    shiftToggleButton->onClick = [this]() {
        shiftMode = !shiftMode;
        updateRowButtonColors();
    };

    for (auto& btn : rowButtons) {
        rightPanel.items.add(juce::FlexItem(*btn).withFlex(1).withMargin(juce::FlexItem::Margin(4)));
    }

    downButtons.items.add(juce::FlexItem(*playToggleButton).withFlex(1));
    downButtons.items.add(juce::FlexItem(*stopToggleButton).withFlex(1));
    downButtons.items.add(juce::FlexItem(*shiftToggleButton).withFlex(1));

    addAndMakeVisible(playToggleButton.get());
    addAndMakeVisible(stopToggleButton.get());
    addAndMakeVisible(shiftToggleButton.get());
    for(auto& button : rowButtons) {
        addAndMakeVisible(button);
    }
}

void ToggleSquareShift::resized() {
    auto bounds = getLocalBounds();

    rightPanel.flexDirection = juce::FlexBox::Direction::column;
    rightPanel.justifyContent = juce::FlexBox::JustifyContent::center;
    rightPanel.alignItems = juce::FlexBox::AlignItems::stretch;

    downButtons.flexDirection = juce::FlexBox::Direction::row;
    downButtons.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    downButtons.alignItems = juce::FlexBox::AlignItems::center;

    rightPanel.performLayout(bounds.toFloat()));
    downButtons.performLayout(bounds.toFloat())



    auto downButtonsBounds = playToggleButton->getBounds();
    int squareSize = downButtonsBounds.getHeight();

    // Apply square size to buttons
    playToggleButton->setSize(squareSize, squareSize);
    stopToggleButton->setSize(squareSize, squareSize);
    shiftToggleButton->setSize(squareSize, squareSize);

    // Reposition buttons within downButtons flexbox
    shiftToggleButton->setBounds(downButtonsBounds.getX() + (downButtonsBounds.getWidth() / 2) - (squareSize / 2),
                                 downButtonsBounds.getY()  + squareSize * 2, squareSize, squareSize);
    playToggleButton->setBounds(downButtonsBounds.getX() + (downButtonsBounds.getWidth() / 2) - (squareSize / 2),
                                downButtonsBounds.getY(), squareSize, squareSize);
    stopToggleButton->setBounds(downButtonsBounds.getX() + (downButtonsBounds.getWidth() / 2) - (squareSize / 2),
                                downButtonsBounds.getY()  + squareSize, squareSize, squareSize);
}
void ToggleSquareShift::updateRowButtonColors() {
    for (auto& btn : rowButtons) {
        if (shiftMode) {
            btn->setColour(juce::TextButton::buttonColourId, shiftToggleButton->findColour(juce::TextButton::buttonColourId));
        } else {
            btn->setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
        }
    }
}