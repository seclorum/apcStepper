#pragma once
#include "Common.h"

class tdSongList : public juce::Component, public juce::DragAndDropTarget {
public:
    std::function<void(tdSongList*, tdSongList*)> onItemDropped;
    std::function<void(tdSongList*)> onDeleteButtonClicked;

    tdSongList(bool isRange = false);
    ~tdSongList() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
    bool isInterestedInDragSource(const SourceDetails&) override;
    void itemDragEnter(const SourceDetails&) override;
    void itemDragExit(const SourceDetails&) override;
    void itemDropped(const SourceDetails&) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;

private:
    juce::ToggleButton toggleButton;
    juce::ComboBox comboBox;
    juce::TextButton deleteButton;
    bool isRangeMode;
    bool isDragOver = false;

    void updateComboBox();
    void deleteButtonClicked();
};
class CustomListComponent : public juce::Component, public juce::DragAndDropTarget {
public:
    CustomListComponent();
    ~CustomListComponent() override = default;

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
};