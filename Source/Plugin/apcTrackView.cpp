#pragma once

#include <juce_gui_basics/juce_gui_basics.h> // For juce::Component, juce::MouseListener, etc.
#include <tracktion_engine/tracktion_engine.h> // For tracktion_engine::MidiClip
// #include "Common.h" // Uncomment if Common.h provides additional necessary includes

class apcTrackView : public juce::Component, private juce::MouseListener
{
public:
    apcTrackView();
    ~apcTrackView() override;

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void setMidiClip(tracktion_engine::MidiClip* clip);

private:
    tracktion_engine::MidiClip* midiClip = nullptr;
    juce::Point<float> dragStart;
    juce::Rectangle<float> selectionRange;
    bool isDragging = false;
    static constexpr float pixelsPerBeat = 100.0f;

    juce::Rectangle<float> getClipBounds() const;
    void selectMIDINotesInRange();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(apcTrackView)
};