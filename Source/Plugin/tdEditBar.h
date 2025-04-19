//
// Created by Tom Peak Walcher on 19.04.25.
//
#ifndef TD_EDIT_BAR_H
#define TD_EDIT_BAR_H

#include "Common.h"
#include <random>

// Main component with FlexBox and buttons
class tdEditBar : public juce::Component, public juce::DragAndDropContainer
{
public:
    tdEditBar()
    {
        // Setup buttons
        addButton.setButtonText("+");
        duplicateButton.setButtonText("|");
        duplicateButton.setEnabled(false); // Disabled until a rectangle exists

        addButton.onClick = [this] { addRectangle(); };
        duplicateButton.onClick = [this] { duplicateLastRectangle(); };

        addAndMakeVisible(addButton);
        addAndMakeVisible(duplicateButton);
        addAndMakeVisible(bar);

        // Set bar background color (using ID 0 for older JUCE)
        bar.setColour(0, juce::Colours::black);

        setSize(800, 200);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
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

        // Center the buttonBox using alignItems (compatible with older JUCE)
        mainBox.alignItems = juce::FlexBox::AlignItems::center;
        mainBox.items.add(juce::FlexItem(buttonBox).withHeight(40).withFlex(0));

        mainBox.performLayout(getLocalBounds().toFloat());

        updateBarLayout();
    }

    void addRectangle();
    void duplicateLastRectangle();
    void reorderRectangles(class ColorRectangle* source, class ColorRectangle* target);
    void updateBarLayout(); // Declare, define later

private:
    // Custom bar component to paint background
    class BarComponent : public juce::Component
    {
    public:
        void paint(juce::Graphics& g) override
        {
            g.fillAll(findColour(0)); // Use color ID 0 for background
        }
    };

    juce::TextButton addButton, duplicateButton;
    BarComponent bar; // Use custom BarComponent
    juce::OwnedArray<class ColorRectangle> rectangles;
};

// Rectangle component with random material color
class ColorRectangle : public juce::Component, public juce::DragAndDropTarget
{
public:
    ColorRectangle()
    {
        // Generate random material color
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
                container->startDragging("ColorRectangle", this, juce::Image(), true);
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
            // Find the tdEditBar parent
            tdEditBar* parent = findParentComponentOfClass<tdEditBar>();
            if (parent)
                parent->reorderRectangles(sourceRect, this);
        }
        repaint();
    }

    juce::Colour getColor() const { return color; }

    void setColor(juce::Colour c)
    {
        color = c;
        repaint();
    }

private:
    juce::Colour color;
    bool isDragging = false;
    bool isDragOver = false;
    juce::Point<int> dragStartPos;
};

// Define tdEditBar methods
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
        newRect->setColor(lastRect->getColor()); // Set color directly
        rectangles.add(newRect);
        bar.addAndMakeVisible(*newRect);
        updateBarLayout();
    }
}

inline void tdEditBar::reorderRectangles(ColorRectangle* source, ColorRectangle* target)
{
    int sourceIndex = rectangles.indexOf(source);
    int targetIndex = rectangles.indexOf(target);

    if (sourceIndex >= 0 && targetIndex >= 0)
    {
        rectangles.remove(sourceIndex);
        rectangles.insert(targetIndex, source);
        updateBarLayout();
    }
}

inline void tdEditBar::updateBarLayout()
{
    juce::Grid grid;
    grid.templateColumns = { juce::Grid::Fr(1) }; // Single row
    grid.templateRows = { juce::Grid::TrackInfo(juce::Grid::Fr(1)) };

    juce::GridItem barGridItem(bar);
    barGridItem.column = { 1, 2 };
    barGridItem.row = { 1, 2 };
    grid.items.add(barGridItem);

    juce::FlexBox rectBox;
    rectBox.flexDirection = juce::FlexBox::Direction::row;
    rectBox.alignItems = juce::FlexBox::AlignItems::stretch;

    // Calculate max width for each rectangle (1/10th of bar width)
    float maxRectWidth = bar.getWidth() / 10.0f;

    for (auto* rect : rectangles)
    {
        // Explicitly create FlexItem and set associatedComponent with cast
        juce::FlexItem flexItem;
        flexItem.associatedComponent = static_cast<juce::Component*>(rect); // Correct cast to Component*
        flexItem.flexGrow = 1.0f;
        flexItem.width = maxRectWidth;
        flexItem.height = bar.getHeight();
        rectBox.items.add(flexItem);
    }

    rectBox.performLayout(bar.getLocalBounds().toFloat());
    bar.repaint();
}

#endif // TD_EDIT_BAR_H