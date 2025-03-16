#pragma once

#include "Common.h"

#include "ShiftToggleButton.h"
#include "apcToggleButton.h"
#include "ToggleIconButton.h"
#include "apcTempoPanel.h"

class apcStepperMainProcessor;

class apcRightPanel : public juce::AudioProcessorEditor {
public:
    apcRightPanel(apcStepperMainProcessor &p)
        : AudioProcessorEditor(&p), processor(p) {
        //Track Buttons with Shift option were generated
        for (int i = 0; i < rows; ++i) {
            rowButtons.add(
                std::make_unique<ShiftToggleButton>(juce::Colours::grey, juce::Colours::blue, juce::Colours::orange,
                                                    false));
            addAndMakeVisible(rowButtons.getLast());
            rowButtons[i]->onClick = [this, i]() { if (!this->shiftMode) squareClicked(i); };
            rightPanel.items.add(juce::FlexItem(*rowButtons.getLast()).withFlex(1).withMargin(6));
        }

        tempoPanel = std::make_unique<apcTempoPanel>(processor);
        addAndMakeVisible(tempoPanel.get());
        playToggleButton = std::make_unique<ToggleIconButton>(juce::Colours::green, juce::Colours::darkgreen,
                                                              BinaryData::playcircle_svg,
                                                              BinaryData::playcircle_svgSize);
        addAndMakeVisible(playToggleButton.get());

        stopToggleButton = std::make_unique<ToggleIconButton>(juce::Colours::red, juce::Colours::darkgrey,
                                                              BinaryData::stopcircle_svg,
                                                              BinaryData::stopcircle_svgSize);
        addAndMakeVisible(stopToggleButton.get());

        shiftToggleButton = std::make_unique<ToggleIconButton>(juce::Colours::blue, juce::Colours::orange,
                                                               BinaryData::shift_svg, BinaryData::shift_svgSize);
        addAndMakeVisible(shiftToggleButton.get());

        shiftToggleButton->onClick = [this]() {
            shiftMode = !shiftMode;

            updateRowButtonColors();
        };
        playToggleButton->onClick = [this]() {
            auto midiOutputDevices = juce::MidiOutput::getAvailableDevices();
            processor.midiOutput= juce::MidiOutput::openDevice(midiOutputDevices[0].identifier);
            APCLOG(midiOutputDevices[0].identifier);
            };

        downButtons.items.add(juce::FlexItem(*tempoPanel).withFlex(1));
        downButtons.items.add(juce::FlexItem(*playToggleButton).withFlex(1));
        downButtons.items.add(juce::FlexItem(*stopToggleButton).withFlex(1));
        downButtons.items.add(juce::FlexItem(*shiftToggleButton).withFlex(1));



        for (auto &button: rowButtons) {
            addAndMakeVisible(button);
        };
        rightPanelContainer.items.add(juce::FlexItem(rightPanel).withFlex(4));
        rightPanelContainer.items.add(juce::FlexItem(downButtons).withFlex(2));
        rightPanelContainer.performLayout(getLocalBounds().toFloat());
        APCLOG("RightPanel initialized...");
;     }

    void resized() override {
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

        tempoPanel->setBounds(tempoPanel->getX(),tempoPanel->getY(),bounds.getWidth(), tempoPanel->getHeight());
        playToggleButton->setBounds(playToggleButton->getX() + (bounds.getWidth() / 2) - (squareSize / 2),
                                    playToggleButton->getY(), squareSize, squareSize);
        stopToggleButton->setBounds(stopToggleButton->getX() + (bounds.getWidth() / 2) - (squareSize / 2),
                                    stopToggleButton->getY(), squareSize, squareSize);
        shiftToggleButton->setBounds(shiftToggleButton->getX() + (bounds.getWidth() / 2) - (squareSize / 2),
                                     shiftToggleButton->getY(), squareSize, squareSize);

    }

private:
    static constexpr int rows = 8;
    juce::OwnedArray<ShiftToggleButton> rowButtons;
    std::unique_ptr<apcTempoPanel> tempoPanel;
    std::unique_ptr<apcToggleButton> playToggleButton;
    std::unique_ptr<apcToggleButton> stopToggleButton;
    std::unique_ptr<apcToggleButton> shiftToggleButton;
    juce::FlexBox rightPanel;
    juce::FlexBox downButtons;
    juce::FlexBox rightPanelContainer;
    bool shiftMode = false;
    apcStepperMainProcessor &processor;;

    void squareClicked(int index) {
        for (int i = 0; i < rows; ++i) {
            if (i != index) {
                rowButtons[i]->setToggleState(false, false);
            }
        }
        rowButtons[index]->setToggleState(true, false);
    }

    void updateRowButtonColors() {
        for (auto &button: rowButtons) {
            button->setShift(shiftMode);
        }
    }
};
