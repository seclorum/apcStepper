#include "tdSongListComponent.h"
#include "tdLookAndFeel.h"

// =========================
// ===== TD SONG LIST ======
// =========================

tdSongList::tdSongList(bool isRange) : isRangeMode(isRange) {
    DBG("tdSongList::tdSongList");
    toggleButton.setButtonText(isRange ? "Range" : "Pattern");
    toggleButton.onClick = [this] {
        isRangeMode = !isRangeMode;
        toggleButton.setButtonText(isRangeMode ? "Range" : "Pattern");
        updateComboBox();
    };
    toggleLookAndFeel = std::make_unique<tdLookAndFeel>();
    toggleButton.setLookAndFeel(toggleLookAndFeel.get());
    addAndMakeVisible(toggleButton);

    comboBox.setTextWhenNothingSelected("Select...");
    comboBox.setJustificationType(juce::Justification::centredLeft);
    comboLookAndFeel = std::make_unique<tdLookAndFeel>();
    comboBox.setLookAndFeel(comboLookAndFeel.get());
    addAndMakeVisible(comboBox);

    deleteButton.setButtonText("-");
    deleteButton.onClick = [this] { deleteButtonClicked(); };
    addAndMakeVisible(deleteButton);

    updateComboBox();
    setSize(200, 80);
    setOpaque(true);
}

tdSongList::~tdSongList() {
    toggleButton.setLookAndFeel(nullptr);
    comboBox.setLookAndFeel(nullptr);
}

void tdSongList::paint(juce::Graphics& g) {
    DBG("tdSongList::paint");
    g.fillAll(juce::Colour::fromFloatRGBA(0.47f, 0.27f, 0.27f, 0.44f));
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);
    if (isDragOver) {
        g.setColour(juce::Colours::yellow.withAlpha(0.3f));
        g.fillRect(getLocalBounds());
        g.setColour(juce::Colours::yellow);
        g.drawRect(getLocalBounds(), 2);
    }
}

void tdSongList::resized() {
    DBG("tdSongList::resized");
    auto bounds = getLocalBounds().reduced(5);
    bounds.setHeight(80);
    setSize(std::max(getWidth(), 200), 80);
    auto deleteButtonBounds = bounds.removeFromRight(30).withHeight(30);
    deleteButton.setBounds(deleteButtonBounds);
    auto buttonBounds = bounds.removeFromLeft(100).withHeight(30);
    toggleButton.setBounds(buttonBounds);
    auto comboBoxBounds = bounds.withHeight(30);
    comboBox.setBounds(comboBoxBounds);
}

bool tdSongList::isInterestedInDragSource(const SourceDetails& details) {
    return details.description.toString() == "ListItem";
}

void tdSongList::itemDragEnter(const SourceDetails&) {
    isDragOver = true;
    repaint();
}

void tdSongList::itemDragExit(const SourceDetails&) {
    isDragOver = false;
    repaint();
}

void tdSongList::itemDropped(const SourceDetails& details) {
    isDragOver = false;
    if (onItemDropped) {
        onItemDropped(this, dynamic_cast<tdSongList*>(details.sourceComponent.get()));
    }
    repaint();
}

void tdSongList::mouseDown(const juce::MouseEvent& e) {
    if (e.mods.isLeftButtonDown()) {
        DBG("tdSongList::mouseDown - Clicked on " << getName());
    }
}

void tdSongList::mouseDrag(const juce::MouseEvent& e) {
    if (!e.mods.isLeftButtonDown() || e.getDistanceFromDragStart() < 5) {
        return;
    }
    auto* container = findParentComponentOfClass<juce::DragAndDropContainer>();
    if (!container) {
        DBG("tdSongList::mouseDrag - Error: No DragAndDropContainer parent found for " << getName());
        return;
    }
    juce::Image snapshot = createComponentSnapshot(getLocalBounds());
    juce::ScaledImage dragImage(snapshot, 1.0f);
    container->startDragging(juce::var("ListItem"), this, dragImage, true, nullptr, &e.source);
}

void tdSongList::updateComboBox() {
    DBG("tdSongList::updateComboBox");
    comboBox.clear();
    if (isRangeMode) {
        comboBox.addItem("1", 1);
        comboBox.addItem("2", 2);
        comboBox.addItem("3", 3);
    } else {
        comboBox.addItem("A", 1);
        comboBox.addItem("B", 2);
        comboBox.addItem("C", 3);
        comboBox.addItem("D", 4);
    }
    comboBox.setSelectedId(1);
}

void tdSongList::deleteButtonClicked() {
    if (onDeleteButtonClicked) {
        onDeleteButtonClicked(this);
    }
}

// =========================
// ===== CUSTOM LIST =======
// =========================


tdSongListComponent::tdSongListComponent() {
    DBG("tdSongListComponent::tdSongListComponent");
    content = std::make_unique<juce::Component>();
    content->setName("ContentComponent");

    viewport.setViewedComponent(content.get(), false);
    viewport.setScrollBarsShown(true, false);
    viewport.setScrollBarThickness(10);
    viewport.setName("Viewport");
    addAndMakeVisible(viewport);

    addButton = std::make_unique<juce::TextButton>("[+]");
    addButton->setName("AddButton");
    addButton->setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    addButton->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    addButton->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    addButton->onClick = [this] {
        DBG("Add button clicked");
        addNewItem();
    };
    content->addAndMakeVisible(*addButton);

    DBG("Adding initial items");
    addNewItem();
    addNewItem();
    addNewItem();
}

void tdSongListComponent::paint(juce::Graphics& g) {
    DBG("tdSongListComponent::paint");
    g.fillAll(juce::Colours::black.withAlpha(0.44f));
    if (isDragOver) {
        g.setColour(juce::Colours::yellow.withAlpha(0.3f));
        g.fillRect(getLocalBounds());
        g.setColour(juce::Colours::yellow);
        g.drawRect(getLocalBounds(), 2);
    }
}

void tdSongListComponent::resized() {
    DBG("tdSongListComponent::resized");
    viewport.setBounds(getLocalBounds().reduced(5));
    updateLayout();
}

void tdSongListComponent::updateLayout() {
    DBG("tdSongListComponent::updateLayout");
    const int itemHeight = 80;
    const int itemSpacing = 5;
    const int buttonHeight = 40;
    const int margin = 5;

    int contentWidth = viewport.getWidth() - viewport.getScrollBarThickness();
    if (contentWidth <= 0) {
        contentWidth = 300;
        DBG("Warning: Viewport width is 0, using fallback width 300");
    }
    contentWidth = std::max(contentWidth, 200);

    int y = margin;
    for (auto* item : items) {
        if (item) {
            item->setBounds(margin, y, contentWidth - 2 * margin, itemHeight);
            item->setVisible(true);
            y += itemHeight + itemSpacing;
        }
    }

    if (addButton) {
        addButton->setBounds(margin, y, contentWidth - 2 * margin, buttonHeight);
        addButton->setVisible(true);
        y += buttonHeight + margin;
    }

    content->setSize(contentWidth, y);
    viewport.setViewPosition(0, 0);
}

void tdSongListComponent::addNewItem() {
    DBG("tdSongListComponent::addNewItem");
    auto* newItem = new tdSongList();
    newItem->onItemDropped = [this](tdSongList* target, tdSongList* source) {
        if (source && target && source != target && items.contains(source) && items.contains(target)) {
            int sourceIndex = items.indexOf(source);
            int targetIndex = items.indexOf(target);
            DBG("Moving item from index " << sourceIndex << " to " << targetIndex);
            items.remove(sourceIndex, false);
            items.insert(targetIndex, source);
            updateLayout();
        }
    };
    newItem->onDeleteButtonClicked = [this](tdSongList* itemToDelete) {
        deleteItem(itemToDelete);
    };
    items.add(newItem);
    content->addAndMakeVisible(newItem);
    newItem->setVisible(true);
    DBG("Added item, total items: " << items.size());
    updateLayout();
}

bool tdSongListComponent::isInterestedInDragSource(const SourceDetails& details) {
    return details.description.toString() == "ListItem";
}

void tdSongListComponent::itemDragEnter(const SourceDetails&) {
    isDragOver = true;
    repaint();
}

void tdSongListComponent::itemDragExit(const SourceDetails&) {
    isDragOver = false;
    repaint();
}

void tdSongListComponent::itemDropped(const SourceDetails& details) {
    isDragOver = false;
    auto* source = dynamic_cast<tdSongList*>(details.sourceComponent.get());
    if (source && items.contains(source)) {
        int sourceIndex = items.indexOf(source);
        items.remove(sourceIndex, false);
        items.add(source);
        DBG("Appended item to end, total items: " << items.size());
        updateLayout();
    }
    repaint();
}

void tdSongListComponent::deleteItem(tdSongList* itemToDelete) {
    if (itemToDelete && items.contains(itemToDelete)) {
        DBG("Deleting item: " << items.indexOf(itemToDelete));
        content->removeChildComponent(itemToDelete);
        items.removeObject(itemToDelete);
        updateLayout();
        repaint();
    }
}