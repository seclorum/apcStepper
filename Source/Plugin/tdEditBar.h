// Created by Tom Peak Walcher on 19.04.25.
#ifndef TD_EDIT_BAR_H
#define TD_EDIT_BAR_H

#include "Common.h"
#include <random>
#include <vector>

class tdEditBar; // Forward declaration

class GreyRectangle : public juce::Component, public juce::DragAndDropTarget {
public:
    GreyRectangle(juce::Colour greyTone, juce::String label, juce::Font font, tdEditBar* parent);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent&) override;
    bool isInterestedInDragSource(const SourceDetails&) override;
    void itemDragEnter(const SourceDetails&) override;
    void itemDragExit(const SourceDetails&) override;
    void itemDropped(const SourceDetails& dragSourceDetails) override;

    juce::Colour getColor() const;
    void setColor(juce::Colour c);
    juce::String getLabel() const;
    void setLabel(const juce::String& newLabel);

private:
    juce::Colour color;
    juce::String textLabel;
    juce::Font fontToUse;
    bool isDragging = false;
    bool isDragOver = false;
    juce::Point<int> dragStartPos;
    tdEditBar* parentBar;

    // Buttons and editor
    juce::TextButton addButton{ "+" };
    juce::TextButton duplicateButton{ "|" };
    juce::TextButton deleteButton{ "-" };
    juce::TextButton editButton{ "Edit" };
    juce::TextEditor labelEditor; // Inline editor for label
};
class tdEditBar
    : public juce::Component
    , public juce::DragAndDropContainer {
public:
    class BarComponent : public juce::Component {
    public:
        void paint(juce::Graphics& g) override;
    };

    tdEditBar();

    void paint(juce::Graphics& g) override;
    void resized() override;

    void addRectangle(int insertIndex = -1); // Modified to support specific index
    void duplicateRectangle(GreyRectangle* source, int insertIndex);
    void removeRectangle(GreyRectangle* rect);
    void reorderRectangles(GreyRectangle* source, GreyRectangle* target);
    void updateBarLayout();

    const juce::OwnedArray<GreyRectangle>& getRectangles() const;
    BarComponent& getBarComponent();

private:
    class DraggableRangeSelector : public juce::Component, public juce::ChangeBroadcaster {
    public:
        DraggableRangeSelector(tdEditBar* parentBar)
            : parent(parentBar), startIndex(0), endIndex(0),
              leftHandle(std::make_unique<LeftHandle>(this)),
              rightHandle(std::make_unique<RightHandle>(this)) {
            setInterceptsMouseClicks(false, true);
            addAndMakeVisible(leftHandle.get());
            addAndMakeVisible(rightHandle.get());
            resized();
        }

        void paint(juce::Graphics& g) override {
            g.fillAll(juce::Colours::black);
            g.setColour(juce::Colours::grey);
            if (startIndex >= 0 && endIndex >= 0) {
                float startX = getPositionOfIndex(startIndex);
                float endX = getPositionOfIndex(endIndex + 1);
                g.fillRect(startX, 0.0f, endX - startX, static_cast<float>(getHeight()));
                g.setColour(juce::Colours::lightgrey);
                g.fillRect(startX, 0.0f, 2.0f, static_cast<float>(getHeight()));
                g.fillRect(endX - 2.0f, 0.0f, 2.0f, static_cast<float>(getHeight()));
            }
        }

        void resized() override {
            if (startIndex >= 0 && endIndex >= 0) {
                float startX = getPositionOfIndex(startIndex);
                float endX = getPositionOfIndex(endIndex + 1);
                float handleWidth = 20.0f;
                leftHandle->setBounds(juce::roundToInt(startX - handleWidth / 2.0f), 0, juce::roundToInt(handleWidth), getHeight());
                rightHandle->setBounds(juce::roundToInt(endX - handleWidth / 2.0f), 0, juce::roundToInt(handleWidth), getHeight());
            }
        }

        int getStartIndex() const { return startIndex; }
        int getEndIndex() const { return endIndex; }

        void addChangeListener(juce::ChangeListener* listener) {
            juce::ChangeBroadcaster::addChangeListener(listener);
        }

        void removeChangeListener(juce::ChangeListener* listener) {
            juce::ChangeBroadcaster::removeChangeListener(listener);
        }

    private:
        class LeftHandle : public juce::Component {
        public:
            LeftHandle(DraggableRangeSelector* selector) : parent(selector) {
                setInterceptsMouseClicks(true, false);
            }

            void paint(juce::Graphics& g) override {
                g.setColour(juce::Colours::lightgrey);
                g.fillRect(getWidth() / 2 - 1, 0, 2, getHeight());
            }

            void mouseDown(const juce::MouseEvent& e) override {
                if (e.mods.isLeftButtonDown()) {
                    isDragging = true;
                }
            }

            void mouseDrag(const juce::MouseEvent& e) override {
                if (isDragging && parent->parent) {
                    float mouseX = static_cast<float>(e.getScreenX() - parent->getScreenX());
                    int newIndex = parent->getIndexAtPosition(mouseX);
                    float minWidth = parent->parent->getBarComponent().getWidth() / 10.0f;
                    float endX = parent->getPositionOfIndex(parent->endIndex + 1);
                    float startX = parent->getPositionOfIndex(newIndex);
                    if (endX - startX >= minWidth) {
                        if (newIndex != parent->startIndex) {
                            parent->startIndex = juce::jlimit(0, parent->endIndex, newIndex);
                            parent->resized();
                            parent->repaint();
                            parent->sendChangeMessage();
                        }
                    }
                }
            }

            void mouseUp(const juce::MouseEvent&) override {
                isDragging = false;
                parent->repaint();
            }

        private:
            DraggableRangeSelector* parent;
            bool isDragging = false;
        };

        class RightHandle : public juce::Component {
        public:
            RightHandle(DraggableRangeSelector* selector) : parent(selector) {
                setInterceptsMouseClicks(true, false);
            }

            void paint(juce::Graphics& g) override {
                g.setColour(juce::Colours::lightgrey);
                g.fillRect(getWidth() / 2 - 1, 0, 2, getHeight());
            }

            void mouseDown(const juce::MouseEvent& e) override {
                if (e.mods.isLeftButtonDown()) {
                    isDragging = true;
                }
            }

            void mouseDrag(const juce::MouseEvent& e) override {
                if (isDragging && parent->parent) {
                    float mouseX = static_cast<float>(e.getScreenX() - parent->getScreenX());
                    int newIndex = parent->getIndexAtPosition(mouseX);
                    float minWidth = parent->parent->getBarComponent().getWidth() / 10.0f;
                    float startX = parent->getPositionOfIndex(parent->startIndex);
                    float endX = parent->getPositionOfIndex(newIndex + 1);
                    int maxIndex = parent->parent->getRectangles().size();
                    if (endX - startX >= minWidth) {
                        if (newIndex != parent->endIndex) {
                            parent->endIndex = juce::jlimit(parent->startIndex, maxIndex, newIndex);
                            parent->resized();
                            parent->repaint();
                            parent->sendChangeMessage();
                        }
                    }
                }
            }

            void mouseUp(const juce::MouseEvent&) override {
                isDragging = false;
                parent->repaint();
            }

        private:
            DraggableRangeSelector* parent;
            bool isDragging = false;
        };

        tdEditBar* parent;
        int startIndex;
        int endIndex;
        std::unique_ptr<LeftHandle> leftHandle;
        std::unique_ptr<RightHandle> rightHandle;

        float getPositionOfIndex(int index) const {
            if (parent) {
                const auto& rectangles = parent->getRectangles();
                float barWidth = static_cast<float>(parent->getBarComponent().getWidth());
                float rectWidth = barWidth / 10.0f;
                if (index >= 0 && index <= rectangles.size()) {
                    return index * rectWidth;
                }
                return index < 0 ? 0.0f : rectWidth * rectangles.size();
            }
            return 0.0f;
        }

        int getIndexAtPosition(float x) const {
            if (parent) {
                float barWidth = static_cast<float>(parent->getBarComponent().getWidth());
                float rectWidth = barWidth / 10.0f;
                if (rectWidth > 0) {
                    int index = juce::roundToInt(x / rectWidth);
                    return juce::jlimit(0, static_cast<int>(parent->getRectangles().size()), index);
                }
            }
            return 0;
        }
    };

    DraggableRangeSelector rangeSelector;
    BarComponent bar;
    juce::OwnedArray<GreyRectangle> rectangles;
    juce::Typeface::Ptr typeface;
    juce::Font customFont;
    int rectangleCount = 0; // Tracks total rectangles added
};

// Implementation of GreyRectangle methods
inline GreyRectangle::GreyRectangle(juce::Colour greyTone, juce::String label, juce::Font font, tdEditBar* parent)
    : color(greyTone), textLabel(label), fontToUse(font), parentBar(parent) {
    setInterceptsMouseClicks(true, true);
    addAndMakeVisible(addButton);
    addAndMakeVisible(duplicateButton);
    addAndMakeVisible(deleteButton);
    addAndMakeVisible(editButton);
    addAndMakeVisible(labelEditor);

    // Initialize editor
    labelEditor.setText(textLabel);
    labelEditor.setVisible(false);
    labelEditor.setFont(fontToUse);
    labelEditor.setJustification(juce::Justification::centred);
    labelEditor.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
    labelEditor.setColour(juce::TextEditor::textColourId, juce::Colours::black);

    // Button callbacks
    addButton.onClick = [this] {
        int index = parentBar->getRectangles().indexOf(this);
        parentBar->addRectangle(index + 1);
    };
    duplicateButton.onClick = [this] {
        int index = parentBar->getRectangles().indexOf(this);
        parentBar->duplicateRectangle(this, index + 1);
    };
    deleteButton.onClick = [this] {
        parentBar->removeRectangle(this);
    };
    editButton.onClick = [this] {
        labelEditor.setVisible(true);
        labelEditor.setText(textLabel); // Reset to current label
        labelEditor.grabKeyboardFocus();
    };

    // Editor callbacks
    labelEditor.onReturnKey = [this] {
        juce::String newLabel = labelEditor.getText();
        if (!newLabel.isEmpty()) {
            setLabel(newLabel);
        }
        labelEditor.setVisible(false);
    };
    labelEditor.onEscapeKey = [this] {
        labelEditor.setVisible(false); // Cancel editing
    };
    labelEditor.onFocusLost = [this] {
        juce::String newLabel = labelEditor.getText();
        if (!newLabel.isEmpty()) {
            setLabel(newLabel);
        }
        labelEditor.setVisible(false);
    };
}

inline void GreyRectangle::paint(juce::Graphics& g) {
    g.fillAll(color);
    g.setColour(juce::Colours::white);
    g.setFont(fontToUse);
    // Draw label in top portion if editor is not visible
    if (!labelEditor.isVisible()) {
        g.drawText(textLabel, getLocalBounds().removeFromTop(30).toFloat(), juce::Justification::centred);
    }
    if (isDragging) {
        g.setColour(juce::Colours::white.withAlpha(0.5f));
        g.fillAll();
    }
}

inline void GreyRectangle::resized() {
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::column;
    fb.justifyContent = juce::FlexBox::JustifyContent::center;
    fb.items.add(juce::FlexItem(addButton).withWidth(30).withHeight(20).withMargin(2));
    fb.items.add(juce::FlexItem(duplicateButton).withWidth(30).withHeight(20).withMargin(2));
    fb.items.add(juce::FlexItem(deleteButton).withWidth(30).withHeight(20).withMargin(2));
    fb.items.add(juce::FlexItem(editButton).withWidth(30).withHeight(20).withMargin(2));
    fb.performLayout(getLocalBounds().withTop(30).reduced(5));

    // Position editor in label area
    labelEditor.setBounds(getLocalBounds().removeFromTop(30).reduced(5));
}

inline void GreyRectangle::mouseDown(const juce::MouseEvent& e) {
    if (e.mods.isLeftButtonDown() && !addButton.getBounds().contains(e.getPosition()) &&
        !duplicateButton.getBounds().contains(e.getPosition()) &&
        !deleteButton.getBounds().contains(e.getPosition()) &&
        !editButton.getBounds().contains(e.getPosition()) &&
        !labelEditor.getBounds().contains(e.getPosition())) {
        isDragging = true;
        dragStartPos = e.getPosition();
        auto* container = findParentComponentOfClass<juce::DragAndDropContainer>();
        if (container) {
            APCLOG("Starting drag for GreyRectangle");
            container->startDragging("GreyRectangle", this, juce::Image(), true);
        }
        else {
            APCLOG("Error: No DragAndDropContainer found");
        }
        repaint();
    }
}

inline void GreyRectangle::mouseUp(const juce::MouseEvent&) {
    isDragging = false;
    repaint();
}

inline bool GreyRectangle::isInterestedInDragSource(const SourceDetails&) {
    return true;
}

inline void GreyRectangle::itemDragEnter(const SourceDetails&) {
    isDragOver = true;
    repaint();
}

inline void GreyRectangle::itemDragExit(const SourceDetails&) {
    isDragOver = false;
    repaint();
}

inline void GreyRectangle::itemDropped(const SourceDetails& dragSourceDetails) {
    isDragOver = false;
    auto* sourceRect = dynamic_cast<GreyRectangle*>(dragSourceDetails.sourceComponent.get());
    if (sourceRect && sourceRect != this) {
        if (parentBar) {
            parentBar->reorderRectangles(sourceRect, this);
        }
        else {
            APCLOG("Error: No tdEditBar parent found in itemDropped");
        }
    }
    repaint();
}

inline juce::Colour GreyRectangle::getColor() const { return color; }

inline void GreyRectangle::setColor(juce::Colour c) {
    color = c;
    repaint();
}

inline juce::String GreyRectangle::getLabel() const { return textLabel; }

inline void GreyRectangle::setLabel(const juce::String& newLabel) {
    textLabel = newLabel;
    labelEditor.setText(newLabel); // Sync editor with label
    repaint();
}



// Implementation of tdEditBar methods
inline tdEditBar::tdEditBar() : rangeSelector(this) {
    addAndMakeVisible(rangeSelector);
    addAndMakeVisible(bar);

    bar.setColour(0, juce::Colours::black);

    juce::Typeface::Ptr temp = juce::Typeface::createSystemTypefaceFor(BinaryData::ConthraxSemiBold_otf, BinaryData::ConthraxSemiBold_otfSize);
    if (temp != nullptr) {
        typeface = temp;
        customFont = juce::Font(typeface);
    }

    // Add one initial rectangle
    addRectangle();
}

inline void tdEditBar::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
}

inline void tdEditBar::resized() {
    juce::FlexBox mainBox;
    mainBox.flexDirection = juce::FlexBox::Direction::column;
    mainBox.items.add(juce::FlexItem(rangeSelector).withHeight(30).withMargin(10));
    mainBox.items.add(juce::FlexItem(bar).withFlex(1.0f).withMargin(10));
    mainBox.alignItems = juce::FlexBox::AlignItems::stretch;
    mainBox.performLayout(getLocalBounds().toFloat());

    updateBarLayout();
}

inline void tdEditBar::addRectangle(int insertIndex) {
    int greyValue = rectangleCount * 255.0f / 15;
    if (greyValue > 255) greyValue = 255;
    juce::Colour newGrey(static_cast<juce::uint8>(greyValue),
                         static_cast<juce::uint8>(greyValue),
                         static_cast<juce::uint8>(greyValue),
                         static_cast<juce::uint8>(128));

    juce::String newLabel;
    if (rectangleCount < 26) {
        newLabel = juce::String::charToString('A' + rectangleCount);
    }
    else {
        int first = rectangleCount / 26 - 1;
        int second = rectangleCount % 26;
        newLabel = juce::String::charToString('A' + first) + juce::String::charToString('A' + second);
    }

    auto* rect = new GreyRectangle(newGrey, newLabel, customFont.withHeight(20.0f), this);
    if (insertIndex >= 0 && insertIndex <= rectangles.size()) {
        rectangles.insert(insertIndex, rect);
    }
    else {
        rectangles.add(rect);
    }
    bar.addAndMakeVisible(*rect);
    ++rectangleCount;
    updateBarLayout();
}

inline void tdEditBar::duplicateRectangle(GreyRectangle* source, int insertIndex) {
    if (source) {
        auto* newRect = new GreyRectangle(source->getColor(), source->getLabel(), customFont.withHeight(20.0f), this);
        rectangles.insert(insertIndex, newRect);
        bar.addAndMakeVisible(*newRect);
        updateBarLayout();
    }
}

inline void tdEditBar::removeRectangle(GreyRectangle* rect) {
    if (rect && rectangles.contains(rect)) {
        rectangles.removeObject(rect);
        updateBarLayout();
    }
}

inline void tdEditBar::reorderRectangles(GreyRectangle* source, GreyRectangle* target) {
    int sourceIndex = rectangles.indexOf(source);
    int targetIndex = rectangles.indexOf(target);

    if (sourceIndex >= 0 && targetIndex >= 0 && sourceIndex != targetIndex) {
        rectangles.move(sourceIndex, targetIndex);
        int currentSourceIndexInBar = bar.getIndexOfChildComponent(source);
        int currentTargetIndexInBar = bar.getIndexOfChildComponent(target);

        if (currentSourceIndexInBar != -1 && currentTargetIndexInBar != -1) {
            bar.removeChildComponent(source);
            bar.addChildComponent(source, targetIndex < currentTargetIndexInBar ? currentTargetIndexInBar : currentTargetIndexInBar + 1);
        }

        updateBarLayout();
    }
    else {
        APCLOG("Error: Invalid indices or same element in reorderRectangles");
    }
}

inline void tdEditBar::updateBarLayout() {
    float maxRectWidth = bar.getWidth() / 10.0f;
    int index = 0;
    for (auto* rect : rectangles) {
        rect->setBounds(index * maxRectWidth, 0, maxRectWidth, bar.getHeight());
        ++index;
    }
    bar.repaint();
    rangeSelector.resized(); // Update range selector handles
}

inline const juce::OwnedArray<GreyRectangle>& tdEditBar::getRectangles() const {
    return rectangles;
}

inline tdEditBar::BarComponent& tdEditBar::getBarComponent() {
    return bar;
}

// Implementation of tdEditBar::BarComponent methods
inline void tdEditBar::BarComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black);
}

#endif // TD_EDIT_BAR_H