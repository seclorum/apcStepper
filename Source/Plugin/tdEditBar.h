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

    juce::TextButton addButton{ "+" };
    juce::TextButton duplicateButton{ "|" };
    juce::TextButton deleteButton{ "-" };
    juce::TextButton editButton{ "Edit" };
    juce::TextEditor labelEditor;
};

class CustomViewport : public juce::Viewport {
public:
    CustomViewport() {
        setScrollBarsShown(false, false);
    }

    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colours::black); // Set background color
        juce::Viewport::paint(g); // Call base class paint
    }
};

class tdEditBar
    : public juce::Component
    , public juce::DragAndDropContainer
    , public juce::Timer
    , private juce::ScrollBar::Listener {
public:
    class BarComponent : public juce::Component {
    public:
        BarComponent(tdEditBar* parent) : parentBar(parent) {
            setInterceptsMouseClicks(true, true);
        }

        void paint(juce::Graphics& g) override {
            g.fillAll(juce::Colours::black);
        }

        void mouseDown(const juce::MouseEvent& e) override {
            if (e.mods.isLeftButtonDown() && parentBar) {
                auto* viewport = findParentComponentOfClass<juce::Viewport>();
                if (viewport) {
                    float clickX = e.getPosition().x + viewport->getViewPositionX();
                    int clickedIndex = parentBar->getIndexAtPosition(clickX);
                    parentBar->handleBarClick(clickedIndex);
                }
            }
        }

    private:
        tdEditBar* parentBar;
    };

    tdEditBar() : bar(this) {
        addAndMakeVisible(viewport);
        viewport.setViewedComponent(&bar, false);
        viewport.getHorizontalScrollBar().addListener(this);
        // Customize scrollbar appearance
        auto& scrollBar = viewport.getHorizontalScrollBar();
        scrollBar.setColour(juce::ScrollBar::backgroundColourId, juce::Colours::black);
        scrollBar.setColour(juce::ScrollBar::thumbColourId, juce::Colours::grey);
        for (int sel = 0; sel < 3; ++sel) {
            auto square = std::make_unique<DraggableRangeSelector>(this);
            addAndMakeVisible(*square);
            rangeSelectors.add(std::move(square));
        }

        juce::Typeface::Ptr temp = juce::Typeface::createSystemTypefaceFor(BinaryData::ConthraxSemiBold_otf, BinaryData::ConthraxSemiBold_otfSize);
        if (temp != nullptr) {
            typeface = temp;
            customFont = juce::Font(typeface);
        }

        addRectangle();
        updateBarLayout();
    }

    ~tdEditBar() override {
        viewport.getHorizontalScrollBar().removeListener(this);
    }

    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colours::darkgrey);
    }

    void resized() override {
        juce::FlexBox mainBox;
        mainBox.flexDirection = juce::FlexBox::Direction::column;
        mainBox.alignItems = juce::FlexBox::AlignItems::stretch;
       juce::FlexBox mainBoxContainer;
        mainBoxContainer.flexDirection = juce::FlexBox::Direction::column;
        mainBoxContainer.alignItems = juce::FlexBox::AlignItems::stretch;

        for (auto* square : rangeSelectors) {
            mainBox.items.add(juce::FlexItem(*square).withHeight(20).withMargin(4));
        }
        mainBoxContainer.items.add(juce::FlexItem(mainBox).withFlex(1.0f).withMargin(4)); // No margin
        mainBoxContainer.items.add(juce::FlexItem(viewport).withFlex(2.0f).withMargin(8)); // No margin

        mainBoxContainer.performLayout(getLocalBounds().toFloat());
        updateBarLayout();
    }

    void addRectangle(int insertIndex = -1) {
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

        auto* rect = new GreyRectangle(newGrey, newLabel, customFont.withHeight(16.0f), this); // Smaller font
        if (insertIndex >= 0 && insertIndex <= rectangles.size()) {
            rectangles.insert(insertIndex, rect);
        }
        else {
            rectangles.add(rect);
        }
        bar.addAndMakeVisible(*rect);
        ++rectangleCount;
        APCLOG("Added rectangle: " + newLabel + ", count: " + juce::String(rectangleCount));
        viewport.setScrollBarsShown(false, rectangles.size() > 10);
        updateBarLayout();
    }

    void duplicateRectangle(GreyRectangle* source, int insertIndex) {
        if (source) {
            auto* newRect = new GreyRectangle(source->getColor(), source->getLabel(), customFont.withHeight(16.0f), this);
            rectangles.insert(insertIndex, newRect);
            bar.addAndMakeVisible(*newRect);
            ++rectangleCount;
            APCLOG("Duplicated rectangle at index: " + juce::String(insertIndex));
            viewport.setScrollBarsShown(false, rectangles.size() > 10);
            updateBarLayout();
        }
    }

    void removeRectangle(GreyRectangle* rect) {
        if (rect && rectangles.contains(rect)) {
            rectangles.removeObject(rect);
            APCLOG("Removed rectangle, new count: " + juce::String(rectangles.size()));
            viewport.setScrollBarsShown(false, rectangles.size() > 10);
            updateBarLayout();
        }
    }

    void reorderRectangles(GreyRectangle* source, GreyRectangle* target) {
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

            APCLOG("Reordered rectangles from " + juce::String(sourceIndex) + " to " + juce::String(targetIndex));
            updateBarLayout();
        }
        else {
            APCLOG("Error: Invalid indices or same element in reorderRectangles");
        }
    }

    void updateBarLayout() {
        float maxRectWidth = getWidth() / 10.0f; // Fixed width
        int rectCount = juce::jmax(1, rectangles.size());
        // Match BarComponent height to Viewport's viewable height
        int barHeight = viewport.getMaximumVisibleHeight();
        bar.setSize(maxRectWidth * rectCount, barHeight);
        int index = 0;
        for (auto* rect : rectangles) {
            rect->setBounds(index * maxRectWidth, 0, maxRectWidth, barHeight);
            ++index;
        }
        APCLOG("Updated bar layout: width=" + juce::String(bar.getWidth()) + ", height=" + juce::String(bar.getHeight()) + ", rectCount=" + juce::String(rectangles.size()));
        bar.repaint();
        for (auto* selector : rangeSelectors) {
            selector->resized();
            selector->repaint();
        }
    }

    void handleBarClick(int clickedIndex) {
        if (!rangeSelectors.isEmpty()) {
            auto* selector = rangeSelectors[0];
            int currentLength = selector->getEndIndex() - selector->getStartIndex() + 1;
            int maxIndex = rectangles.size();
            int newStart, newEnd;

            float maxRectWidth = getWidth() / 10.0f;
            float clickX = clickedIndex * maxRectWidth;
            float midPoint = viewport.getViewPositionX() + viewport.getWidth() / 2.0f;

            if (clickX < midPoint) {
                newStart = 0;
                newEnd = juce::jmin(currentLength - 1, maxIndex - 1);
            }
            else {
                newEnd = maxIndex - 1;
                newStart = juce::jmax(0, newEnd - currentLength + 1);
            }

            selector->setRange(newStart, newEnd);
            selector->highlightRange(true);

            float startX = newStart * maxRectWidth;
            float endX = (newEnd + 1) * maxRectWidth;
            float viewWidth = viewport.getWidth();
            if (startX < viewport.getViewPositionX()) {
                viewport.setViewPosition(startX, 0);
            }
            else if (endX > viewport.getViewPositionX() + viewWidth) {
                viewport.setViewPosition(endX - viewWidth, 0);
            }

            APCLOG("Bar click: index=" + juce::String(clickedIndex) + ", new range=[" + juce::String(newStart) + "," + juce::String(newEnd) + "]");
        }
    }

    int getIndexAtPosition(float x) const {
        float maxRectWidth = getWidth() / 10.0f;
        if (maxRectWidth > 0) {
            int index = juce::roundToInt(x / maxRectWidth);
            return juce::jlimit(0, static_cast<int>(rectangles.size()), index);
        }
        return 0;
    }

    const juce::OwnedArray<GreyRectangle>& getRectangles() const {
        return rectangles;
    }

    BarComponent& getBarComponent() {
        return bar;
    }

private:
    void scrollBarMoved(juce::ScrollBar* scrollBarThatHasMoved, double newRangeStart) override {
        //startTimer(100); // Use timer for scroll end detection
        updateBarLayout();
        APCLOG("ScrollBar moved: position=" + juce::String(newRangeStart));
    }

    void timerCallback() override {
        stopTimer();
        updateBarLayout();
        APCLOG("Scroll ended, updated layout");
    }

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
            g.setColour(highlighted ? juce::Colours::lightblue : juce::Colours::grey);
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

        void setRange(int newStart, int newEnd) {
            startIndex = juce::jlimit(0, parent->getRectangles().size(), newStart);
            endIndex = juce::jlimit(startIndex, parent->getRectangles().size(), newEnd);
            resized();
            repaint();
            sendChangeMessage();
        }

        void highlightRange(bool enable) {
            highlighted = enable;
            repaint();
            if (enable) {
                juce::Timer::callAfterDelay(500, [this] {
                    highlighted = false;
                    repaint();
                });
            }
        }

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
        bool highlighted = false;
        std::unique_ptr<LeftHandle> leftHandle;
        std::unique_ptr<RightHandle> rightHandle;

        float getPositionOfIndex(int index) const {
            if (parent) {
                const auto& rectangles = parent->getRectangles();
                float rectWidth = parent->getWidth() / 10.0f;
                auto* viewport = parent->getBarComponent().findParentComponentOfClass<juce::Viewport>();
                float scrollOffset = viewport ? viewport->getViewPositionX() : 0.0f;
                if (index >= 0 && index <= rectangles.size()) {
                    return index * rectWidth - scrollOffset;
                }
                return index < 0 ? -scrollOffset : (rectWidth * rectangles.size() - scrollOffset);
            }
            return 0.0f;
        }

        int getIndexAtPosition(float x) const {
            if (parent) {
                float rectWidth = parent->getWidth() / 10.0f;
                auto* viewport = parent->getBarComponent().findParentComponentOfClass<juce::Viewport>();
                float scrollOffset = viewport ? viewport->getViewPositionX() : 0.0f;
                if (rectWidth > 0) {
                    int index = juce::roundToInt((x + scrollOffset) / rectWidth);
                    return juce::jlimit(0, static_cast<int>(parent->getRectangles().size()), index);
                }
            }
            return 0;
        }
    };

    CustomViewport viewport;
    juce::OwnedArray<DraggableRangeSelector> rangeSelectors;
    BarComponent bar;
    juce::OwnedArray<GreyRectangle> rectangles;
    juce::Typeface::Ptr typeface;
    juce::Font customFont;
    int rectangleCount = 0;
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

    labelEditor.setText(textLabel);
    labelEditor.setVisible(false);
    labelEditor.setFont(fontToUse);
    labelEditor.setJustification(juce::Justification::centred);
    labelEditor.setColour(juce::TextEditor::backgroundColourId, juce::Colours::white);
    labelEditor.setColour(juce::TextEditor::textColourId, juce::Colours::black);

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
        labelEditor.setText(textLabel);
        labelEditor.grabKeyboardFocus();
    };

    labelEditor.onReturnKey = [this] {
        juce::String newLabel = labelEditor.getText();
        if (!newLabel.isEmpty()) {
            setLabel(newLabel);
        }
        labelEditor.setVisible(false);
    };
    labelEditor.onEscapeKey = [this] {
        labelEditor.setVisible(false);
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
    if (!labelEditor.isVisible()) {
        g.drawText(textLabel, getLocalBounds().removeFromTop(20).toFloat(), juce::Justification::centred);
    }
    if (isDragging) {
        g.setColour(juce::Colours::white.withAlpha(0.5f));
        g.fillAll();
    }
}

inline void GreyRectangle::resized() {
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::column;
    fb.justifyContent = juce::FlexBox::JustifyContent::flexStart; // Align to top
    fb.items.add(juce::FlexItem(addButton).withWidth(24).withHeight(16).withMargin(1));
    fb.items.add(juce::FlexItem(duplicateButton).withWidth(24).withHeight(16).withMargin(1));
    fb.items.add(juce::FlexItem(deleteButton).withWidth(24).withHeight(16).withMargin(1));
    fb.items.add(juce::FlexItem(editButton).withWidth(24).withHeight(16).withMargin(1));
    fb.performLayout(getLocalBounds().withTop(20).reduced(5));

    labelEditor.setBounds(getLocalBounds().removeFromTop(20).reduced(5));
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
    labelEditor.setText(newLabel);
    repaint();
}

#endif // TD_EDIT_BAR_H