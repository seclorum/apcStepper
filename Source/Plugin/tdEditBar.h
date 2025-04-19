//
// Created by Tom Peak Walcher on 19.04.25.
//
#ifndef TD_EDIT_BAR_H
#define TD_EDIT_BAR_H

#include "Common.h"
#include <random>
#include <vector>

class tdEditBar; // Forward declaration

class GreyRectangle : public juce::Component, public juce::DragAndDropTarget
{
public:
    GreyRectangle(juce::Colour greyTone, juce::String label, juce::Font font);

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent&) override;
    bool isInterestedInDragSource(const SourceDetails&) override;
    void itemDragEnter(const SourceDetails&) override;
    void itemDragExit(const SourceDetails&) override;
    void itemDropped(const SourceDetails& dragSourceDetails) override;

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
    , public juce::DragAndDropContainer
{
public:
    class BarComponent : public juce::Component
    {
    public:
        void paint(juce::Graphics& g) override;
    };

    tdEditBar();

    void paint(juce::Graphics& g) override;
    void resized() override;

    void addRectangle();
    void duplicateLastRectangle();
    void reorderRectangles(GreyRectangle* source, GreyRectangle* target);
    void updateBarLayout();

    const juce::OwnedArray<GreyRectangle>& getRectangles() const;
    BarComponent& getBarComponent();

private:
    class DraggableRangeSelector : public juce::Component
    {
    public:
        DraggableRangeSelector(tdEditBar* parentBar);

        void paint(juce::Graphics& g) override;
        void resized() override;
        void mouseDown(const juce::MouseEvent& e) override;
        void mouseUp(const juce::MouseEvent&) override;
        void mouseDrag(const juce::MouseEvent& e) override;

        int getStartIndex() const;
        int getEndIndex() const;

        void addChangeListener(juce::ChangeListener* listener);
        void removeChangeListener(juce::ChangeListener* listener);

    private:
        tdEditBar* parent;
        int startIndex;
        int endIndex;
        bool isDragging = false;
        juce::ListenerList<juce::ChangeListener> listeners;

        float getPositionOfIndex(int index) const;
        int getIndexAtPosition(float x) const;
    };

    DraggableRangeSelector rangeSelector;
    juce::TextButton addButton, duplicateButton;
    BarComponent bar;
    juce::OwnedArray<GreyRectangle> rectangles;
    juce::Typeface::Ptr typeface;
    juce::Font customFont;
};

// Implementation of GreyRectangle methods
inline GreyRectangle::GreyRectangle(juce::Colour greyTone, juce::String label, juce::Font font)
    : color(greyTone), textLabel(label), fontToUse(font)
{
    setInterceptsMouseClicks(true, false);
}

inline void GreyRectangle::paint(juce::Graphics& g)
{
    g.fillAll(color);
    g.setColour(juce::Colours::white);
    g.setFont(fontToUse);
    g.drawText(textLabel, getLocalBounds().toFloat(), juce::Justification::centred);
    if (isDragging)
    {
        g.setColour(juce::Colours::white.withAlpha(0.5f));
        g.fillAll();
    }
}

inline void GreyRectangle::mouseDown(const juce::MouseEvent& e)
{
    if (e.mods.isLeftButtonDown())
    {
        isDragging = true;
        dragStartPos = e.getPosition();
        auto* container = findParentComponentOfClass<juce::DragAndDropContainer>();
        if (container)
        {
            APCLOG("Starting drag for GreyRectangle");
            container->startDragging("GreyRectangle", this, juce::Image(), true);
        }
        else
        {
            APCLOG("Error: No DragAndDropContainer found");
        }
        repaint();
    }
}

inline void GreyRectangle::mouseUp(const juce::MouseEvent&)
{
    isDragging = false;
    repaint();
}

inline bool GreyRectangle::isInterestedInDragSource(const SourceDetails&)
{
    return true;
}

inline void GreyRectangle::itemDragEnter(const SourceDetails&)
{
    isDragOver = true;
    repaint();
}

inline void GreyRectangle::itemDragExit(const SourceDetails&)
{
    isDragOver = false;
    repaint();
}

inline void GreyRectangle::itemDropped(const SourceDetails& dragSourceDetails)
{
    isDragOver = false;
    auto* sourceRect = dynamic_cast<GreyRectangle*>(dragSourceDetails.sourceComponent.get());
    if (sourceRect && sourceRect != this)
    {
        tdEditBar* parent = findParentComponentOfClass<tdEditBar>();
        if (parent)
        {
            parent->reorderRectangles(sourceRect, this);
        }
        else
        {
            APCLOG("Error: No tdEditBar parent found in itemDropped");
        }
    }
    repaint();
}

inline juce::Colour GreyRectangle::getColor() const { return color; }
inline void GreyRectangle::setColor(juce::Colour c) { color = c; repaint(); }
inline juce::String GreyRectangle::getLabel() const { return textLabel; }

// Implementation of tdEditBar methods
inline tdEditBar::tdEditBar() : rangeSelector(this)
{
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

    juce::Typeface::Ptr temp = juce::Typeface::createSystemTypefaceFor(BinaryData::ConthraxSemiBold_otf, BinaryData::ConthraxSemiBold_otfSize);
    if (temp != nullptr)
    {
        typeface = temp;
        customFont = juce::Font(typeface);
    }
}

inline void tdEditBar::paint(juce::Graphics& g)
{
    // Remove debug color to avoid covering children
    g.fillAll(juce::Colours::darkgrey);
}

inline void tdEditBar::resized()
{
    juce::FlexBox mainBox;
    mainBox.flexDirection = juce::FlexBox::Direction::column;
    mainBox.items.add(juce::FlexItem(rangeSelector).withHeight(30).withMargin(5));
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

inline void tdEditBar::addRectangle()
{
    int greyValue = static_cast<int>(rectangles.size() * 255.0f / 9.0f);
    if (greyValue > 255) greyValue = 255;
    if (greyValue < 0) greyValue = 0;
    juce::Colour newGrey(static_cast<juce::uint8>(greyValue),
                         static_cast<juce::uint8>(greyValue),
                         static_cast<juce::uint8>(greyValue),
                         static_cast<juce::uint8>(128));

    juce::String newLabel;
    if (rectangles.size() < 26)
    {
        newLabel = String::charToString('A' + rectangles.size());
    }
    else
    {
        newLabel = 'A' + String::charToString('A' + rectangles.size());
    }
    auto* rect = new GreyRectangle(newGrey, newLabel, customFont.withHeight(20.0f));
    rectangles.add(rect);
    bar.addAndMakeVisible(*rect);
    duplicateButton.setEnabled(true);
    updateBarLayout();
}

inline void tdEditBar::duplicateLastRectangle()
{
    if (!rectangles.isEmpty())
    {
        auto* lastRect = rectangles.getLast();
        auto* newRect = new GreyRectangle(lastRect->getColor(), lastRect->getLabel(), customFont.withHeight(20.0f));
        rectangles.add(newRect);
        bar.addAndMakeVisible(*newRect);
        updateBarLayout();
    }
}

inline void tdEditBar::reorderRectangles(GreyRectangle* source, GreyRectangle* target)
{
    int sourceIndex = rectangles.indexOf(source);
    int targetIndex = rectangles.indexOf(target);

    if (sourceIndex >= 0 && targetIndex >= 0 && sourceIndex != targetIndex)
    {
        rectangles.move(sourceIndex, targetIndex); // Reorder the OwnedArray

        // Explicitly reorder the children of the bar component
        int currentSourceIndexInBar = bar.getIndexOfChildComponent(source);
        int currentTargetIndexInBar = bar.getIndexOfChildComponent(target);

        if (currentSourceIndexInBar != -1 && currentTargetIndexInBar != -1)
        {
            bar.removeChildComponent(source);
            bar.addChildComponent(source, targetIndex < currentTargetIndexInBar ? currentTargetIndexInBar : currentTargetIndexInBar + 1);
        }

        updateBarLayout();
    }
    else
    {
        APCLOG("Error: Invalid indices or same element in reorderRectangles");
    }
}

inline void tdEditBar::updateBarLayout()
{
    float maxRectWidth = bar.getWidth() / 10.0f;
    int index = 0;
    for (auto* rect : rectangles)
    {
        rect->setBounds(index * maxRectWidth, 0, maxRectWidth, bar.getHeight());
        ++index;
    }
    bar.repaint();
}

inline const juce::OwnedArray<GreyRectangle>& tdEditBar::getRectangles() const
{
    return rectangles;
}

inline tdEditBar::BarComponent& tdEditBar::getBarComponent()
{
    return bar;
}

// Implementation of tdEditBar::BarComponent methods
inline void tdEditBar::BarComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

// Implementation of DraggableRangeSelector methods
inline tdEditBar::DraggableRangeSelector::DraggableRangeSelector(tdEditBar* parentBar)
    : parent(parentBar)
    , startIndex(-1)
    , endIndex(-1)
{
    setInterceptsMouseClicks(true, true);
}

inline void tdEditBar::DraggableRangeSelector::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::lightgrey);
    g.setColour(juce::Colours::blue.withAlpha(0.5f));
    if (startIndex != -1 && endIndex != -1)
    {
        float startX = getPositionOfIndex(startIndex);
        float endX = getPositionOfIndex(endIndex + 1); // +1 to cover the end boundary
        g.fillRect(startX, 0.0f, endX - startX, static_cast<float>(getHeight()));
    }
}

inline void tdEditBar::DraggableRangeSelector::resized()
{
    // No specific resizing needed for the visual representation itself
}

inline void tdEditBar::DraggableRangeSelector::mouseDown(const juce::MouseEvent& e)
{
    if (parent)
    {
        float mouseX = static_cast<float>(e.getPosition().getX());
        startIndex = getIndexAtPosition(mouseX);
        endIndex = startIndex;
        repaint();
        isDragging = true;
    }
}

inline void tdEditBar::DraggableRangeSelector::mouseUp(const juce::MouseEvent&)
{
    isDragging = false;
    if (startIndex > endIndex)
        std::swap(startIndex, endIndex);
    repaint();
}

inline void tdEditBar::DraggableRangeSelector::mouseDrag(const juce::MouseEvent& e)
{
    if (isDragging && parent)
    {
        float mouseX = static_cast<float>(e.getPosition().getX());
        int currentIndex = getIndexAtPosition(mouseX);
        if (currentIndex != -1)
        {
            endIndex = currentIndex;
            repaint();
        }
    }
}

inline int tdEditBar::DraggableRangeSelector::getStartIndex() const { return startIndex; }
inline int tdEditBar::DraggableRangeSelector::getEndIndex() const { return endIndex; }

inline void tdEditBar::DraggableRangeSelector::addChangeListener(juce::ChangeListener* listener)
{
    listeners.add(listener);
}

inline void tdEditBar::DraggableRangeSelector::removeChangeListener(juce::ChangeListener* listener)
{
    listeners.remove(listener);
}

inline float tdEditBar::DraggableRangeSelector::getPositionOfIndex(int index) const
{
    if (parent)
    {
        const auto& rectangles = parent->getRectangles();
        if (index >= 0 && index <= rectangles.size())
        {
            if (rectangles.isEmpty())
                return 0.0f;
            float barWidth = static_cast<float>(parent->getBarComponent().getWidth());
            float rectWidth = barWidth / 10.0f;
            return (index) * rectWidth;
        }
        else if (rectangles.isEmpty())
        {
            return 0.0f;
        }
        else if (index < 0)
        {
            return 0.0f;
        }
        else
        {
            return static_cast<float>(parent->getBarComponent().getWidth());
        }
    }
    return 0.0f;
}

inline int tdEditBar::DraggableRangeSelector::getIndexAtPosition(float x) const
{
    if (parent)
    {
        const auto& rectangles = parent->getRectangles();
        if (!rectangles.isEmpty())
        {
            float barWidth = static_cast<float>(parent->getBarComponent().getWidth());
            float rectWidth = barWidth / rectangles.size();
            if (rectWidth > 0)
            {
                int index = juce::roundToInt(x / rectWidth);
                return juce::jlimit(-1, static_cast<int>(rectangles.size()), index);
            }
        }
        return -1;
    }
    return -1;
}

#endif // TD_EDIT_BAR_H