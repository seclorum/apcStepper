#include "apcTempoPanel.h"

class apcRightPanel : public juce::AudioProcessorEditor {
public:
    apcRightPanel(trackDeckMainProcessor &p)
        : AudioProcessorEditor(&p), processor(p) {


        // Initialize FlexBox member variables


        // Add rowButtons
        for (int i = 0; i < rows; ++i) {
            std::string parameterID = "r" + processor.addLeadingZeros(i);
            auto rowButton = std::make_unique<apcShiftToggleParameterButton>(parameterID, i, juce::Colours::grey, juce::Colours::blue, juce::Colours::orange, false, processor);
            addAndMakeVisible(*rowButton);
            rowButtons.add(std::move(rowButton));


        }


        // ... (rest of constructor)
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
            juce::File midiFile("/Users/tompeakwalcher/Documents/tom_first.mid");
            if (midiFile.existsAsFile() && !midiFile.deleteFile())
                APCLOG("Failed to delete existing file!");
            processor.saveMidiFile(midiFile);
        };





    }

    void resized() override {
        auto bounds = getLocalBounds();
        juce::FlexBox rightPanel;         // Member variable
        juce::FlexBox downButtons;        // Member variable
        juce::FlexBox rightPanelContainer;// Member variable
        rightPanel.flexDirection = juce::FlexBox::Direction::column;
        rightPanel.justifyContent = juce::FlexBox::JustifyContent::flexEnd;
        rightPanel.alignItems = juce::FlexBox::AlignItems::flexStart;
        rightPanel.alignContent = juce::FlexBox::AlignContent::spaceBetween;

        rightPanelContainer.flexDirection = juce::FlexBox::Direction::column;
        rightPanelContainer.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
        rightPanelContainer.alignItems = juce::FlexBox::AlignItems::flexStart;

        downButtons.flexDirection = juce::FlexBox::Direction::column;
        downButtons.justifyContent = juce::FlexBox::JustifyContent::flexStart;
        downButtons.alignItems = juce::FlexBox::AlignItems::flexStart;
        downButtons.items.add(juce::FlexItem(*tempoPanel).withFlex(1));
        downButtons.items.add(juce::FlexItem(*playToggleButton).withFlex(1));
        downButtons.items.add(juce::FlexItem(*stopToggleButton).withFlex(1));
        downButtons.items.add(juce::FlexItem(*shiftToggleButton).withFlex(1));
        rightPanelContainer.items.add(juce::FlexItem(rightPanel).withFlex(4).withMargin(8));
        rightPanelContainer.items.add(juce::FlexItem(downButtons).withFlex(2));

        for (auto& button : rowButtons) {
            rightPanel.items.add(juce::FlexItem(*button).withMaxHeight(getHeight()).withWidth(getWidth()/1.4).withMargin(juce::FlexItem::Margin(10)).withFlex(1));
            // Set width dynamically
        }
        rightPanelContainer.performLayout(getLocalBounds().toFloat());

        // Size and position other components manually if needed

        int squareSize = playToggleButton->getHeight();
        playToggleButton->setSize(squareSize, squareSize);
        stopToggleButton->setSize(squareSize, squareSize);
        shiftToggleButton->setSize(squareSize, squareSize);

        tempoPanel->setBounds(tempoPanel->getX(), tempoPanel->getY(), bounds.getWidth(), tempoPanel->getHeight());
        playToggleButton->setBounds(playToggleButton->getX() + (bounds.getWidth() / 2) - (squareSize / 2),
                                    playToggleButton->getY(), squareSize, squareSize);
        stopToggleButton->setBounds(stopToggleButton->getX() + (bounds.getWidth() / 2) - (squareSize / 2),
                                    stopToggleButton->getY(), squareSize, squareSize);
        shiftToggleButton->setBounds(shiftToggleButton->getX() + (bounds.getWidth() / 2) - (squareSize / 2),
                                     shiftToggleButton->getY(), squareSize, squareSize);


        APCLOG("RightPanel initialized...");
    }
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

private:
    static constexpr int rows = 8;
    juce::OwnedArray<apcShiftToggleParameterButton> rowButtons;
    std::unique_ptr<apcTempoPanel> tempoPanel;
    std::unique_ptr<apcToggleButton> playToggleButton;
    std::unique_ptr<apcToggleButton> stopToggleButton;
    std::unique_ptr<apcToggleButton> shiftToggleButton;

    bool shiftMode = false;
    trackDeckMainProcessor &processor;
};
