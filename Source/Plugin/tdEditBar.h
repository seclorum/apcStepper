// Created by Tom Peak Walcher on 19.04.25.
#ifndef TD_EDIT_BAR_H
#define TD_EDIT_BAR_H

#include "Common.h"
#include <random>
#include <vector>

class tdEditBar; // Forward declaration

class GreyRectangle : public juce::Component, public juce::DragAndDropTarget {
public:
    GreyRectangle(juce::Colour greyTone, juce::String label, juce::Font font);

    void paint(juce::Graphics &g) override;

    void mouseDown(const juce::MouseEvent &e) override;

    void mouseUp(const juce::MouseEvent &) override;

    bool isInterestedInDragSource(const SourceDetails &) override;

    void itemDragEnter(const SourceDetails &) override;

    void itemDragExit(const SourceDetails &) override;

    void itemDropped(const SourceDetails &dragSourceDetails) override;

    juce::Colour getColor() const;

    void setColor(juce::Colour c);

    juce::String getLabel() const;

private:
    juce::Colour color;
    juce::String textLabel;
    juce::Font fontToUse;
    bool isDragging = false;
    bool isDragOver = false;
    juce::Point<int> dragStartPos;
};

class tdEditBar
        : public juce::Component
          , public juce::DragAndDropContainer {
public:
    class BarComponent : public juce::Component {
    public:
        void paint(juce::Graphics &g) override;
    };

    tdEditBar();

    void paint(juce::Graphics &g) override;

    void resized() override;

    void addRectangle();

    void duplicateLastRectangle();

    void reorderRectangles(GreyRectangle *source, GreyRectangle *target);

    void updateBarLayout();

    const juce::OwnedArray<GreyRectangle> &getRectangles() const;

    BarComponent &getBarComponent();

private:
    class DraggableRangeSelector : public juce::Component, public juce::ChangeBroadcaster {
    public:
        DraggableRangeSelector(tdEditBar *parentBar)
            : parent(parentBar), startIndex(0), endIndex(0),
              leftHandle(std::make_unique<LeftHandle>(this)),
              rightHandle(std::make_unique<RightHandle>(this)) {
            setInterceptsMouseClicks(false, true); // Allow handle events only
            addAndMakeVisible(leftHandle.get());
            addAndMakeVisible(rightHandle.get());
            resized(); // Position handles at startup
        }

        void paint(juce::Graphics &g) override {
            // Background: dark gray
            g.fillAll(juce::Colours::black);
            // Range fill: light blue
            g.setColour(juce::Colours::grey);
            if (startIndex >= 0 && endIndex >= 0) {
                float startX = getPositionOfIndex(startIndex);
                float endX = getPositionOfIndex(endIndex + 1); // +1 to cover the end boundary
                g.fillRect(startX, 0.0f, endX - startX, static_cast<float>(getHeight()));
                // Vertical lines at range ends: light gray, 2 pixels wide
                g.setColour(juce::Colours::lightgrey);
                g.fillRect(startX, 0.0f, 2.0f, static_cast<float>(getHeight()));
                g.fillRect(endX - 2.0f, 0.0f, 2.0f, static_cast<float>(getHeight()));
            }
        }

        void resized() override {
            if (startIndex >= 0 && endIndex >= 0) {
                float startX = getPositionOfIndex(startIndex);
                float endX = getPositionOfIndex(endIndex + 1);
                float handleWidth = 20.0f; // 10-pixel draggable area
                // Center handles on range ends
                leftHandle->setBounds(juce::roundToInt(startX - handleWidth / 2.0f), 0, juce::roundToInt(handleWidth),
                                      getHeight());
                rightHandle->setBounds(juce::roundToInt(endX - handleWidth / 2.0f), 0, juce::roundToInt(handleWidth),
                                       getHeight());
            }
        }

        int getStartIndex() const { return startIndex; }
        int getEndIndex() const { return endIndex; }

        void addChangeListener(juce::ChangeListener *listener) {
            juce::ChangeBroadcaster::addChangeListener(listener);
        }

        void removeChangeListener(juce::ChangeListener *listener) {
            juce::ChangeBroadcaster::removeChangeListener(listener);
        }

    private:
        class LeftHandle : public juce::Component {
        public:
            LeftHandle(DraggableRangeSelector *selector) : parent(selector) {
                setInterceptsMouseClicks(true, false);
            }

            void paint(juce::Graphics &g) override {
                g.setColour(juce::Colours::lightgrey);
                // Line at handle center (aligns with startX)
                g.fillRect(getWidth() / 2 - 1, 0, 2, getHeight());
            }

            void mouseDown(const juce::MouseEvent &e) override {
                if (e.mods.isLeftButtonDown()) {
                    isDragging = true;
                }
            }

            void mouseDrag(const juce::MouseEvent &e) override {
                if (isDragging && parent->parent) {
                    // Use screen coordinates relative to parent
                    float mouseX = static_cast<float>(e.getScreenX() - parent->getScreenX());
                    int newIndex = parent->getIndexAtPosition(mouseX);
                    float minWidth = parent->parent->getBarComponent().getWidth() / 10.0f;
                    float endX = parent->getPositionOfIndex(parent->endIndex + 1);
                    float startX = parent->getPositionOfIndex(newIndex);
                    if (endX - startX >= minWidth) {
                        if (newIndex != parent->startIndex) {
                            parent->startIndex = juce::jlimit(0, parent->endIndex, newIndex);
                            parent->resized(); // Update handle positions
                            parent->repaint();
                            parent->sendChangeMessage();
                        }
                    }
                }
            }

            void mouseUp(const juce::MouseEvent &) override {
                isDragging = false;
                parent->repaint(); // Ensure final state is clear
            }

        private:
            DraggableRangeSelector *parent;
            bool isDragging = false;
        };

        class RightHandle : public juce::Component {
        public:
            RightHandle(DraggableRangeSelector *selector) : parent(selector) {
                setInterceptsMouseClicks(true, false);
            }

            void paint(juce::Graphics &g) override {
                g.setColour(juce::Colours::lightgrey);
                // Line at handle center (aligns with endX)
                g.fillRect(getWidth() / 2 - 1, 0, 2, getHeight());
            }

            void mouseDown(const juce::MouseEvent &e) override {
                if (e.mods.isLeftButtonDown()) {
                    isDragging = true;
                }
            }

            void mouseDrag(const juce::MouseEvent &e) override {
                if (isDragging && parent->parent) {
                    // Use screen coordinates relative to parent
                    float mouseX = static_cast<float>(e.getScreenX() - parent->getScreenX());
                    int newIndex = parent->getIndexAtPosition(mouseX);
                    float minWidth = parent->parent->getBarComponent().getWidth() / 10.0f;
                    float startX = parent->getPositionOfIndex(parent->startIndex);
                    float endX = parent->getPositionOfIndex(newIndex + 1);
                    int maxIndex = parent->parent->getRectangles().size();
                    if (endX - startX >= minWidth) {
                        if (newIndex != parent->endIndex) {
                            parent->endIndex = juce::jlimit(parent->startIndex, maxIndex, newIndex);
                            parent->resized(); // Update handle positions
                            parent->repaint();
                            parent->sendChangeMessage();
                        }
                    }
                }
            }

            void mouseUp(const juce::MouseEvent &) override {
                isDragging = false;
                parent->repaint(); // Ensure final state is clear
            }

        private:
            DraggableRangeSelector *parent;
            bool isDragging = false;
        };

        tdEditBar *parent;
        int startIndex;
        int endIndex;
        std::unique_ptr<LeftHandle> leftHandle;
        std::unique_ptr<RightHandle> rightHandle;

        float getPositionOfIndex(int index) const {
            if (parent) {
                const auto &rectangles = parent->getRectangles();
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
    juce::TextButton addButton, duplicateButton;
    BarComponent bar;
    juce::OwnedArray<GreyRectangle> rectangles;
    juce::Typeface::Ptr typeface;
    juce::Font customFont;
    int rectangleCount = 0; // Tracks total rectangles added
};

// Implementation of GreyRectangle methods (unchanged)
inline GreyRectangle::GreyRectangle(juce::Colour greyTone, juce::String label, juce::Font font)
    : color(greyTone), textLabel(label), fontToUse(font) {
    setInterceptsMouseClicks(true, false);
}

inline void GreyRectangle::paint(juce::Graphics &g) {
    g.fillAll(color);
    g.setColour(juce::Colours::white);
    g.setFont(fontToUse);
    g.drawText(textLabel, getLocalBounds().toFloat(), juce::Justification::centred);
    if (isDragging) {
        g.setColour(juce::Colours::white.withAlpha(0.5f));
        g.fillAll();
    }
}

inline void GreyRectangle::mouseDown(const juce::MouseEvent &e) {
    if (e.mods.isLeftButtonDown()) {
        isDragging = true;
        dragStartPos = e.getPosition();
        auto *container = findParentComponentOfClass<juce::DragAndDropContainer>();
        if (container) {
            APCLOG("Starting drag for GreyRectangle");
            container->startDragging("GreyRectangle", this, juce::Image(), true);
        } else {
            APCLOG("Error: No DragAndDropContainer found");
        }
        repaint();
    }
}

inline void GreyRectangle::mouseUp(const juce::MouseEvent &) {
    isDragging = false;
    repaint();
}

inline bool GreyRectangle::isInterestedInDragSource(const SourceDetails &) {
    return true;
}

inline void GreyRectangle::itemDragEnter(const SourceDetails &) {
    isDragOver = true;
    repaint();
}

inline void GreyRectangle::itemDragExit(const SourceDetails &) {
    isDragOver = false;
    repaint();
}

inline void GreyRectangle::itemDropped(const SourceDetails &dragSourceDetails) {
    isDragOver = false;
    auto *sourceRect = dynamic_cast<GreyRectangle *>(dragSourceDetails.sourceComponent.get());
    if (sourceRect && sourceRect != this) {
        tdEditBar *parent = findParentComponentOfClass<tdEditBar>();
        if (parent) {
            parent->reorderRectangles(sourceRect, this);
        } else {
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

// Implementation of tdEditBar methods
inline tdEditBar::tdEditBar() : rangeSelector(this) {
    addButton.setButtonText("+");
    duplicateButton.setButtonText("|");
    duplicateButton.setEnabled(false);

    addButton.onClick = [this] { addRectangle(); };
    duplicateButton.onClick = [this] { duplicateLastRectangle(); };

    addAndMakeVisible(rangeSelector);
    addAndMakeVisible(addButton);
    addAndMakeVisible(duplicateButton);
    addAndMakeVisible(bar);

    bar.setColour(0, juce::Colours::black);

    juce::Typeface::Ptr temp = juce::Typeface::createSystemTypefaceFor(BinaryData::ConthraxSemiBold_otf,
                                                                       BinaryData::ConthraxSemiBold_otfSize);
    if (temp != nullptr) {
        typeface = temp;
        customFont = juce::Font(typeface);
    }

    // Add one initial rectangle
    addRectangle();
}

inline void tdEditBar::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::darkgrey);
}

inline void tdEditBar::resized() {
    juce::FlexBox mainBox;
    mainBox.flexDirection = juce::FlexBox::Direction::column;
    mainBox.items.add(juce::FlexItem(rangeSelector).withHeight(30).withMargin(10));
    mainBox.items.add(juce::FlexItem(bar).withFlex(1.0f).withMargin(10));

    juce::FlexBox buttonBox;
    buttonBox.flexDirection = juce::FlexBox::Direction::row;
    buttonBox.items.add(juce::FlexItem(addButton).withWidth(50).withHeight(30).withMargin(5));
    buttonBox.items.add(juce::FlexItem(duplicateButton).withWidth(50).withHeight(30).withMargin(5));

    mainBox.alignItems = juce::FlexBox::AlignItems::stretch;
    mainBox.items.add(juce::FlexItem(buttonBox).withHeight(40).withFlex(0));

    mainBox.performLayout(getLocalBounds().toFloat());

    updateBarLayout();
}

inline void tdEditBar::addRectangle() {
    int greyValue = rectangleCount * 255.0f / 15;
    if (greyValue > 255) greyValue = 255;
    juce::Colour newGrey(static_cast<juce::uint8>(greyValue),
                         static_cast<juce::uint8>(greyValue),
                         static_cast<juce::uint8>(greyValue),
                         static_cast<juce::uint8>(128));

    juce::String newLabel;
    if (rectangleCount < 26) {
        newLabel = juce::String::charToString('A' + rectangleCount);
    } else {
        int first = rectangleCount / 26 - 1;
        int second = rectangleCount % 26;
        newLabel = juce::String::charToString('A' + first) + juce::String::charToString('A' + second);
    }

    auto *rect = new GreyRectangle(newGrey, newLabel, customFont.withHeight(20.0f));
    rectangles.add(rect);
    bar.addAndMakeVisible(*rect);
    duplicateButton.setEnabled(true);
    ++rectangleCount;
    updateBarLayout();
}

inline void tdEditBar::duplicateLastRectangle() {
    if (!rectangles.isEmpty()) {
        auto *lastRect = rectangles.getLast();
        auto *newRect = new GreyRectangle(lastRect->getColor(), lastRect->getLabel(), customFont.withHeight(20.0f));
        rectangles.add(newRect);
        bar.addAndMakeVisible(*newRect);
        ++rectangleCount;
        updateBarLayout();
    }
}

inline void tdEditBar::reorderRectangles(GreyRectangle *source, GreyRectangle *target) {
    int sourceIndex = rectangles.indexOf(source);
    int targetIndex = rectangles.indexOf(target);

    if (sourceIndex >= 0 && targetIndex >= 0 && sourceIndex != targetIndex) {
        rectangles.move(sourceIndex, targetIndex);
        int currentSourceIndexInBar = bar.getIndexOfChildComponent(source);
        int currentTargetIndexInBar = bar.getIndexOfChildComponent(target);

        if (currentSourceIndexInBar != -1 && currentTargetIndexInBar != -1) {
            bar.removeChildComponent(source);
            bar.addChildComponent(source, targetIndex < currentTargetIndexInBar
                                              ? currentTargetIndexInBar
                                              : currentTargetIndexInBar + 1);
        }

        updateBarLayout();
    } else {
        APCLOG("Error: Invalid indices or same element in reorderRectangles");
    }
}

inline void tdEditBar::updateBarLayout() {
    float maxRectWidth = bar.getWidth() / 10.0f;
    int index = 0;
    for (auto *rect: rectangles) {
        rect->setBounds(index * maxRectWidth, 0, maxRectWidth, bar.getHeight());
        ++index;
    }
    bar.repaint();
}

inline const juce::OwnedArray<GreyRectangle> &tdEditBar::getRectangles() const {
    return rectangles;
}

inline tdEditBar::BarComponent &tdEditBar::getBarComponent() {
    return bar;
}

// Implementation of tdEditBar::BarComponent methods
inline void tdEditBar::BarComponent::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::black);
}

#endif // TD_EDIT_BAR_H
