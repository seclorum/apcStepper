// MidiControls.h
// MIDI control interface for trackDeck
// Copyright (c) Raw Material Software Limited
// Licensed under the ISC license: http://www.isc.org/downloads/software-support-policy/isc-license

#pragma once

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <tracktion_engine/tracktion_engine.h>

namespace trackDeck
{

//==============================================================================
    struct MidiDeviceListEntry final : juce::ReferenceCountedObject
    {
        explicit MidiDeviceListEntry(juce::MidiDeviceInfo info) : deviceInfo(info) {}

        juce::MidiDeviceInfo deviceInfo;
        std::unique_ptr<juce::MidiInput> inDevice;
        std::unique_ptr<juce::MidiOutput> outDevice;

        using Ptr = juce::ReferenceCountedObjectPtr<MidiDeviceListEntry>;

        void stopAndReset();
    };

//==============================================================================
    class MidiControls final : public juce::Component,
                               private juce::MidiKeyboardState::Listener,
                               private juce::MidiInputCallback,
                               private juce::AsyncUpdater
    {
    public:
        explicit MidiControls(tracktion_engine::Engine& engine);
        ~MidiControls() override;

        void paint(juce::Graphics& g) override;
        void resized() override;

        int getNumMidiInputs() const noexcept;
        int getNumMidiOutputs() const noexcept;
        MidiDeviceListEntry::Ptr getMidiDevice(int index, bool isInput) const noexcept;

    private:
        class MidiDeviceListBox final : private juce::ListBoxModel, public juce::ListBox
        {
        public:
            MidiDeviceListBox(const juce::String& name, MidiControls& owner, bool isInputDeviceList);
            int getNumRows() override;
            void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
            void selectedRowsChanged(int lastRow) override;
            void syncSelectedItemsWithDeviceList(const juce::ReferenceCountedArray<MidiDeviceListEntry>& midiDevices);

        private:
            MidiControls& parent;
            bool isInput;
            juce::SparseSet<int> lastSelectedItems;
        };

        void handleNoteOn(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
        void handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
        void handleIncomingMidiMessage(juce::MidiInput*, const juce::MidiMessage& message) override;
        void handleAsyncUpdate() override;

        void openDevice(bool isInput, int index);
        void closeDevice(bool isInput, int index);
        void sendToOutputs(const juce::MidiMessage& msg);
        bool hasDeviceListChanged(const juce::Array<juce::MidiDeviceInfo>& availableDevices, bool isInputDevice) const;
        MidiDeviceListEntry::Ptr findDevice(juce::MidiDeviceInfo device, bool isInputDevice) const;
        void closeUnpluggedDevices(const juce::Array<juce::MidiDeviceInfo>& currentlyPluggedInDevices, bool isInputDevice);
        void updateDeviceList(bool isInputDeviceList);
        void updateDeviceLists();
        void addLabelAndSetStyle(juce::Label& label);

        tracktion_engine::Engine& engine;
        juce::Label midiInputLabel{"Midi Input Label", "MIDI Input:"};
        juce::Label midiOutputLabel{"Midi Output Label", "MIDI Output:"};
        juce::Label incomingMidiLabel{"Incoming Midi Label", "Received MIDI messages:"};
        juce::Label outgoingMidiLabel{"Outgoing Midi Label", "Play the keyboard to send MIDI messages..."};
        juce::MidiKeyboardState keyboardState;
        juce::MidiKeyboardComponent midiKeyboard;
        juce::TextEditor midiMonitor{"MIDI Monitor"};
        juce::TextButton pairButton{"MIDI Bluetooth devices..."};
        juce::ReferenceCountedArray<MidiDeviceListEntry> midiInputs, midiOutputs;
        std::unique_ptr<MidiDeviceListBox> midiInputSelector, midiOutputSelector;
        juce::CriticalSection midiMonitorLock;
        juce::Array<juce::MidiMessage> incomingMessages;
        juce::MidiDeviceListConnection connection;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiControls)
    };

} // namespace trackDeck