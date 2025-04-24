#pragma once
#include "Common.h"

class tdLookAndFeel;

class tdSongList : public juce::Component, public juce::DragAndDropTarget {
public:
    tdSongList(bool isRange = false);
    ~tdSongList() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    bool isInterestedInDragSource(const SourceDetails&) override;
    void itemDragEnter(const SourceDetails&) override;
    void itemDragExit(const SourceDetails&) override;
    void itemDropped(const SourceDetails&) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;

    std::function<void(tdSongList*, tdSongList*)> onItemDropped;
    std::function<void(tdSongList*)> onDeleteButtonClicked;

private:
    juce::ToggleButton toggleButton;
    juce::ComboBox comboBox;
    juce::TextButton deleteButton;
    bool isRangeMode;
    bool isDragOver = false;
    std::unique_ptr<tdLookAndFeel> toggleLookAndFeel;
    std::unique_ptr<tdLookAndFeel> comboLookAndFeel;

    void updateComboBox();
    void deleteButtonClicked();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(tdSongList)
};

class tdSongListComponent : public juce::Component, public juce::DragAndDropTarget {
public:
    tdSongListComponent();
    ~tdSongListComponent() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void addNewItem();
    void updateLayout();
    bool isInterestedInDragSource(const SourceDetails&) override;
    void itemDragEnter(const SourceDetails&) override;
    void itemDragExit(const SourceDetails&) override;
    void itemDropped(const SourceDetails&) override;

private:
    juce::Viewport viewport;
    std::unique_ptr<juce::Component> content;
    std::unique_ptr<juce::TextButton> addButton;
    juce::OwnedArray<tdSongList> items;
    bool isDragOver = false;

    void deleteItem(tdSongList* itemToDelete);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(tdSongListComponent)
};