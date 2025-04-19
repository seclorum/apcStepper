//
// Created by Tom Peak Walcher on 19.04.25.
//
#ifndef TD_EDIT_BAR_H
#define TD_EDIT_BAR_H

#include "Common.h"
#include <random>

class tdEditBar : public juce::Component, public juce::DragAndDropContainer
{
public:
    tdEditBar()
    {
        addButton.setButtonText("+");
        duplicateButton.setButtonText("|");
        duplicateButton.setEnabled(false);

        addButton.onClick = [this] { addRectangle(); };
        duplicateButton.onClick = [this] { duplicateLastRectangle(); };

        addAndMakeVisible(addButton);
        addAndMakeVisible(duplicateButton);
        addAndMakeVisible(bar);

        bar.setColour(0, juce::Colours::black);
    }

    void paint(juce::Graphics& g) override
    {
        // Remove debug color to avoid covering children
        // g.fillAll(juce::Colours::darkgrey);
    }

    void resized() override
    {
        juce::FlexBox mainBox;
        mainBox.flexDirection = juce::FlexBox::Direction::column;
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

    void addRectangle();
    void duplicateLastRectangle();
    void reorderRectangles(class ColorRectangle* source, class ColorRectangle* target);
    void updateBarLayout();

private:
    class BarComponent : public juce::Component
    {
    public:
        void paint(juce::Graphics& g) override
        {
            g.fillAll(juce::Colours::black);
        }
    };

    juce::TextButton addButton, duplicateButton;
    BarComponent bar;
    juce::OwnedArray<class ColorRectangle> rectangles;
};

class ColorRectangle : public juce::Component, public juce::DragAndDropTarget
{
public:
    ColorRectangle()
    {
        static const juce::Colour materialColors[] = {
            juce::Colour(0xFFF44336), juce::Colour(0xFFE91E63), juce::Colour(0xFF9C27B0),
            juce::Colour(0xFF673AB7), juce::Colour(0xFF3F51B5), juce::Colour(0xFF2196F3),
            juce::Colour(0xFF03A9F4), juce::Colour(0xFF00BCD4), juce::Colour(0xFF009688),
            juce::Colour(0xFF4CAF50)
        };
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(0, 9);
        color = materialColors[dis(gen)];
        setInterceptsMouseClicks(true, false);
        setOpaque(true);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(color);
        if (isDragging)
        {
            g.setColour(juce::Colours::white.withAlpha(0.5f));
            g.fillAll();
        }
    }

    void mouseDown(const juce::MouseEvent& e) override
    {
        if (e.mods.isLeftButtonDown())
        {
            isDragging = true;
            dragStartPos = e.getPosition();
            auto* container = findParentComponentOfClass<juce::DragAndDropContainer>();
            if (container)
            {
                APCLOG("Starting drag for ColorRectangle");
                container->startDragging("ColorRectangle", this, juce::Image(), true);
            }
            else
            {
                APCLOG("Error: No DragAndDropContainer found");
            }
            repaint();
        }
    }

    void mouseUp(const juce::MouseEvent&) override
    {
        isDragging = false;
        repaint();
    }

    bool isInterestedInDragSource(const SourceDetails&) override
    {
        return true;
    }

    void itemDragEnter(const SourceDetails&) override
    {
        isDragOver = true;
        repaint();
    }

    void itemDragExit(const SourceDetails&) override
    {
        isDragOver = false;
        repaint();
    }

    void itemDropped(const SourceDetails& dragSourceDetails) override
    {
        isDragOver = false;
        auto* sourceRect = dynamic_cast<ColorRectangle*>(dragSourceDetails.sourceComponent.get());
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

    juce::Colour getColor() const { return color; }
    void setColor(juce::Colour c) { color = c; repaint(); }

private:
    juce::Colour color;
    bool isDragging = false;
    bool isDragOver = false;
    juce::Point<int> dragStartPos;
};

inline void tdEditBar::addRectangle()
{
    auto* rect = new ColorRectangle();
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
        auto* newRect = new ColorRectangle();
        newRect->setColor(lastRect->getColor());
        rectangles.add(newRect);
        bar.addAndMakeVisible(*newRect);
        updateBarLayout();
    }
}

inline void tdEditBar::reorderRectangles(ColorRectangle* source, ColorRectangle* target)
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

#endif // TD_EDIT_BAR_H