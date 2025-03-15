//
// Created by Jay Vaughan on 12.03.25.
//
//
// Created by Jay Vaughan on 12.03.25.
//

#include "apcLog.h"

const int apcLog::buttonItemId;

apcLog::apcLog()
{
    auto imageInputStream = std::make_unique<juce::MemoryInputStream>(BinaryData::cuttlefish_jpg, BinaryData::cuttlefish_jpgSize, false);
    cuttlefishImage = juce::PNGImageFormat().decodeImage(*imageInputStream);

    addAndMakeVisible(toolbar);
    toolbar.addDefaultItems(*this);

    logTextBox.setMultiLine(true);
    logTextBox.setReadOnly(true);
    logTextBox.setScrollbarsShown(true);
    logTextBox.setCaretVisible(false);
    logTextBox.setPopupMenuEnabled(true);
    addAndMakeVisible(logTextBox);

    logger = std::make_unique<TextBoxLogger>(logTextBox);
    juce::Logger::setCurrentLogger(logger.get());

    juce::Logger::writeToLog("Logger initialized...");
}

apcLog::~apcLog()
{
    juce::Logger::setCurrentLogger(nullptr);
}

void apcLog::logMessage(const juce::String& message)
{
    logTextBox.moveCaretToEnd();
    logTextBox.insertTextAtCaret(message + newLine);
}

void apcLog::getAllToolbarItemIds(juce::Array<int>& ids)
{
    ids.add(buttonItemId);
}

void apcLog::getDefaultItemSet(juce::Array<int>& ids)
{
    ids.add(buttonItemId);
}

juce::ToolbarItemComponent* apcLog::createItem(int itemId)
{
    if (itemId == buttonItemId)
    {
        auto* button = new juce::ToolbarButton(buttonItemId, "CuttlefishButton",
                                               juce::Drawable::createFromImageData(BinaryData::cuttlefish_jpg, BinaryData::cuttlefish_jpgSize), nullptr);

        button->onClick = [this]() {
            logMessage("Cuttlefish button clicked!");
        };

        return button;
    }
    return nullptr;
}

void apcLog::resized()
{
    auto bounds = getLocalBounds();
    toolbar.setBounds(bounds.removeFromTop(40));
    logTextBox.setBounds(bounds);
}
