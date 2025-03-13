//
// Created by Jay Vaughan on 12.03.25.
//

#ifndef APCSEQUENCER_APCLOG_H
#define APCSEQUENCER_APCLOG_H

#include "Common.h"

#define APCLOG(x) Logger::getCurrentLogger()->writeToLog(x)

using namespace juce;

class TextBoxLogger : public juce::Logger
{
public:
    TextBoxLogger(juce::TextEditor& editor)
            : logEditor(editor)
    {}

    void logMessage(const juce::String& message) override
    {
        juce::MessageManager::callAsync([this, message]()
                                        {
                                            logEditor.moveCaretToEnd();
                                            logEditor.insertTextAtCaret(message + newLine);
                                        });
    }

private:
    juce::TextEditor& logEditor;
};

class apcLog final : public Component, public ToolbarItemFactory
{
public:
    apcLog();
    ~apcLog();

    void logMessage(const juce::String& message);

    void getAllToolbarItemIds(juce::Array<int>& ids) override;
    void getDefaultItemSet(juce::Array<int>& ids) override;
    juce::ToolbarItemComponent* createItem(int itemId) override;

    void resized() override;

private:
    juce::Toolbar toolbar;
    juce::TextEditor logTextBox;
    juce::Image cuttlefishImage;
    std::unique_ptr<TextBoxLogger> logger;

    static const int buttonItemId = 1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(apcLog)
};

#endif //APCSEQUENCER_APCLOG_H