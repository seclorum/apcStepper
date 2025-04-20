#include "tdSongList.h"

// =========================
// ===== TD SONG LIST ======
// =========================

tdSongList::tdSongList(bool isRange) : isRangeMode(isRange) {
    DBG("tdSongList::tdSongList");
    // Initialize toggle button
    toggleButton.setButtonText(isRange ? "Range" : "Pattern");
    toggleButton.onClick = [this] {
        isRangeMode = !isRangeMode;
        toggleButton.setButtonText(isRangeMode ? "Range" : "Pattern");
        updateComboBox();
    };
    addAndMakeVisible(toggleButton);

    // Initialize combo box
    comboBox.setTextWhenNothingSelected("Select...");
    comboBox.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(comboBox);

    // Initialize delete button
    deleteButton.setButtonText("-");
    deleteButton.onClick = [this] { deleteButtonClicked(); };
    addAndMakeVisible(deleteButton);

    // Update combo box
    updateComboBox();

    // Set minimum size and properties
    setSize(200, 80);
    setOpaque(true);
}

void tdSongList::paint(juce::Graphics& g) {
    DBG("tdSongList::paint");
    g.fillAll(juce::Colour::fromFloatRGBA(0.47f, 0.27f, 0.27f, 0.44f));
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);

    // Highlight if drop target
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
    DBG("ToggleButton bounds: " << buttonBounds.toString());

    auto comboBoxBounds = bounds.withHeight(30);
    comboBox.setBounds(comboBoxBounds);
    DBG("ComboBox bounds: " << comboBoxBounds.toString());
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
    // Optional: Handle selection or other mouse-down behavior
    if (e.mods.isLeftButtonDown()) {
        DBG("tdSongList::mouseDown - Clicked on " << getName());
    }
}

void tdSongList::mouseDrag(const juce::MouseEvent& e) {
    if (!e.mods.isLeftButtonDown() || e.getDistanceFromDragStart() < 5) {
        return; // Wait for a small drag distance to confirm intent
    }

    auto* container = findParentComponentOfClass<juce::DragAndDropContainer>();
    if (!container) {
        DBG("tdSongList::mouseDrag - Error: No DragAndDropContainer parent found for " << getName());
        return;
    }

    // Create a ScaledImage from the component's appearance
    juce::Image snapshot = createComponentSnapshot(getLocalBounds());
    juce::ScaledImage dragImage(snapshot, 1.0f);

    DBG("tdSongList::mouseDrag - Starting drag for " << getName());
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

CustomListComponent::CustomListComponent() {
    DBG("CustomListComponent::CustomListComponent");
    // Initialize content component
    content = std::make_unique<juce::Component>();
    content->setName("ContentComponent");

    // Configure viewport
    viewport.setViewedComponent(content.get(), false);
    viewport.setScrollBarsShown(true, false);
    viewport.setScrollBarThickness(10);
    viewport.setName("Viewport");
    addAndMakeVisible(viewport);

    // Initialize add button
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

    // Add starting items
    DBG("Adding initial items");
    addNewItem();
    addNewItem();
    addNewItem();
}

void CustomListComponent::resized() {
    DBG("CustomListComponent::resized");
    viewport.setBounds(getLocalBounds().reduced(5));
    updateLayout();
}

void CustomListComponent::updateLayout() {
    DBG("CustomListComponent::updateLayout");
    const int itemHeight = 80;
    const int itemSpacing = 5;
    const int buttonHeight = 40;
    const int margin = 5;

    // Calculate content width
    int contentWidth = viewport.getWidth() - viewport.getScrollBarThickness();
    if (contentWidth <= 0) {
        contentWidth = 300;
        DBG("Warning: Viewport width is 0, using fallback width 300");
    }
    contentWidth = std::max(contentWidth, 200);

    int y = margin;

    // Layout tdSongList items
    DBG("Laying out " << items.size() << " items");
    for (auto* item : items) {
        if (item) {
            item->setBounds(margin, y, contentWidth - 2 * margin, itemHeight);
            item->setVisible(true);
            DBG("Item " << items.indexOf(item) << " bounds: " << item->getBounds().toString());
            y += itemHeight + itemSpacing;
        } else {
            DBG("Error: Null item in items array");
        }
    }

    // Place [+] button
    if (addButton) {
        addButton->setBounds(margin, y, contentWidth - 2 * margin, buttonHeight);
        addButton->setVisible(true);
        DBG("AddButton bounds: " << addButton->getBounds().toString());
        y += buttonHeight + margin;
    } else {
        DBG("Error: addButton is null");
    }

    // Set content size
    content->setSize(contentWidth, y);
    DBG("Content size: " << content->getWidth() << "x" << content->getHeight());
    DBG("Viewport size: " << viewport.getWidth() << "x" << viewport.getHeight());
    DBG("Viewport view position: " << viewport.getViewPosition().toString());

    // Show top of content
    viewport.setViewPosition(0, 0);
}

void CustomListComponent::addNewItem() {
    DBG("CustomListComponent::addNewItem");
    auto* newItem = new tdSongList();
    newItem->onItemDropped = [this](tdSongList* target, tdSongList* source) {
        if (source && target && source != target && items.contains(source) && items.contains(target)) {
            int sourceIndex = items.indexOf(source);
            int targetIndex = items.indexOf(target);
            DBG("Moving item from index " << sourceIndex << " to " << targetIndex);
            items.remove(sourceIndex, false); // Remove without deleting
            items.insert(targetIndex,source);   // Insert at target index
            updateLayout();
        } else {
            DBG("Invalid drag-and-drop: source or target not in items");
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

bool CustomListComponent::isInterestedInDragSource(const SourceDetails& details) {
    return details.description.toString() == "ListItem";
}

void CustomListComponent::itemDragEnter(const SourceDetails&) {
    isDragOver = true;
    repaint();
}

void CustomListComponent::itemDragExit(const SourceDetails&) {
    isDragOver = false;
    repaint();
}

void CustomListComponent::itemDropped(const SourceDetails& details) {
    isDragOver = false;
    auto* source = dynamic_cast<tdSongList*>(details.sourceComponent.get());
    if (source && items.contains(source)) {
        int sourceIndex = items.indexOf(source);
        items.remove(sourceIndex, false); // Remove without deleting
        items.add(source);                // Append to end
        DBG("Appended item to end, total items: " << items.size());
        updateLayout();
    }
    repaint();
}

void CustomListComponent::deleteItem(tdSongList* itemToDelete) {
    if (itemToDelete && items.contains(itemToDelete)) {
        DBG("Deleting item: " << items.indexOf(itemToDelete));
        content->removeChildComponent(itemToDelete);
        items.removeObject(itemToDelete);
        updateLayout();
        repaint();
    } else {
        DBG("Error: Invalid item to delete");
    }
}

void CustomListComponent::paint(juce::Graphics& g) {
    DBG("CustomListComponent::paint");
    g.fillAll(juce::Colours::black.withAlpha(0.44f));
    if (isDragOver) {
        g.setColour(juce::Colours::yellow.withAlpha(0.3f));
        g.fillRect(getLocalBounds());
        g.setColour(juce::Colours::yellow);
        g.drawRect(getLocalBounds(), 2);
    }
}