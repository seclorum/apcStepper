//
// Created by Jay Vaughan on 25.04.25.
//

// MidiControls.cpp
// MIDI control interface implementation for trackDeck
// Copyright (c) Raw Material Software Limited
// Licensed under the ISC license: http://www.isc.org/downloads/software-support-policy/isc-license

#include "MidiControls.h"

namespace trackDeck
{

//==============================================================================
    void MidiDeviceListEntry::stopAndReset()
    {
        if (inDevice)
            inDevice->stop();
        inDevice.reset();
        outDevice.reset();
    }

//==============================================================================
    MidiControls::MidiControls(tracktion_engine::Engine& e)
            : engine(e),
              midiKeyboard(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
              midiInputSelector(std::make_unique<MidiDeviceListBox>("Midi Input Selector", *this, true)),
              midiOutputSelector(std::make_unique<MidiDeviceListBox>("Midi Output Selector", *this, false)),
              connection(juce::MidiDeviceListConnection::make([this] { updateDeviceLists(); }))
    {
        addLabelAndSetStyle(midiInputLabel);
        addLabelAndSetStyle(midiOutputLabel);
        addLabelAndSetStyle(incomingMidiLabel);
        addLabelAndSetStyle(outgoingMidiLabel);

        midiKeyboard.setName("MIDI Keyboard");
        addAndMakeVisible(midiKeyboard);

        midiMonitor.setMultiLine(true);
        midiMonitor.setReadOnly(true);
        midiMonitor.setScrollbarsShown(true);
        midiMonitor.setCaretVisible(false);
        midiMonitor.setPopupMenuEnabled(false);
        addAndMakeVisible(midiMonitor);

        pairButton.setEnabled(juce::BluetoothMidiDevicePairingDialogue::isAvailable());
        pairButton.onClick = [] {
            juce::RuntimePermissions::request(juce::RuntimePermissions::bluetoothMidi,
                                              [](bool granted) {
                                                  if (granted)
                                                      juce::BluetoothMidiDevicePairingDialogue::open();
                                              });
        };
        addAndMakeVisible(pairButton);

        addAndMakeVisible(*midiInputSelector);
        addAndMakeVisible(*midiOutputSelector);

        keyboardState.addListener(this);
        updateDeviceLists();

        setSize(732, 520);
    }

    MidiControls::~MidiControls()
    {
        keyboardState.removeListener(this);
        midiInputs.clear();
        midiOutputs.clear();
        midiInputSelector.reset();
        midiOutputSelector.reset();
    }

    void MidiControls::paint(juce::Graphics&) {}

    void MidiControls::resized()
    {
        constexpr int margin = 10;
        auto bounds = getLocalBounds();

        midiInputLabel.setBounds(margin, margin, (getWidth() / 2) - (2 * margin), 24);
        midiOutputLabel.setBounds((getWidth() / 2) + margin, margin, (getWidth() / 2) - (2 * margin), 24);

        midiInputSelector->setBounds(margin, (2 * margin) + 24,
                                     (getWidth() / 2) - (2 * margin),
                                     (getHeight() / 2) - ((4 * margin) + 24 + 24));
        midiOutputSelector->setBounds((getWidth() / 2) + margin, (2 * margin) + 24,
                                      (getWidth() / 2) - (2 * margin),
                                      (getHeight() / 2) - ((4 * margin) + 24 + 24));

        pairButton.setBounds(margin, (getHeight() / 2) - (margin + 24), getWidth() - (2 * margin), 24);
        outgoingMidiLabel.setBounds(margin, getHeight() / 2, getWidth() - (2 * margin), 24);
        midiKeyboard.setBounds(margin, (getHeight() / 2) + (24 + margin), getWidth() - (2 * margin), 64);

        incomingMidiLabel.setBounds(margin, (getHeight() / 2) + (24 + (2 * margin) + 64),
                                    getWidth() - (2 * margin), 24);
        midiMonitor.setBounds(margin, (getHeight() / 2) + ((2 * 24) + (3 * margin) + 64),
                              getWidth() - (2 * margin), getHeight() - ((getHeight() / 2) + ((2 * 24) + (3 * margin) + 64)) - margin);
    }

    int MidiControls::getNumMidiInputs() const noexcept { return midiInputs.size(); }
    int MidiControls::getNumMidiOutputs() const noexcept { return midiOutputs.size(); }

    MidiDeviceListEntry::Ptr MidiControls::getMidiDevice(int index, bool isInput) const noexcept
    {
        return isInput ? midiInputs[index] : midiOutputs[index];
    }

//==============================================================================
    MidiControls::MidiDeviceListBox::MidiDeviceListBox(const juce::String& name, MidiControls& owner, bool isInputDeviceList)
            : juce::ListBox(name), parent(owner), isInput(isInputDeviceList)
    {
        setModel(this);
        setOutlineThickness(1);
        setMultipleSelectionEnabled(true);
        setClickingTogglesRowSelection(true);
    }

    int MidiControls::MidiDeviceListBox::getNumRows()
    {
        return isInput ? parent.getNumMidiInputs() : parent.getNumMidiOutputs();
    }

    void MidiControls::MidiDeviceListBox::paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool rowIsSelected)
    {
        auto textColour = getLookAndFeel().findColour(juce::ListBox::textColourId);
        if (rowIsSelected)
            g.fillAll(textColour.interpolatedWith(getLookAndFeel().findColour(juce::ListBox::backgroundColourId), 0.5));

        g.setColour(textColour);
        g.setFont(static_cast<float>(height) * 0.7f);

        if (row < (isInput ? parent.getNumMidiInputs() : parent.getNumMidiOutputs()))
            g.drawText(parent.getMidiDevice(row, isInput)->deviceInfo.name,
                       5, 0, width, height, juce::Justification::centredLeft, true);
    }

    void MidiControls::MidiDeviceListBox::selectedRowsChanged(int)
    {
        auto newSelectedItems = getSelectedRows();
        if (newSelectedItems != lastSelectedItems)
        {
            for (auto i : lastSelectedItems)
                if (!newSelectedItems.contains(i))
                    parent.closeDevice(isInput, i);

            for (auto i : newSelectedItems)
                if (!lastSelectedItems.contains(i))
                    parent.openDevice(isInput, i);

            lastSelectedItems = newSelectedItems;
        }
    }

    void MidiControls::MidiDeviceListBox::syncSelectedItemsWithDeviceList(const juce::ReferenceCountedArray<MidiDeviceListEntry>& midiDevices)
    {
        juce::SparseSet<int> selectedRows;
        for (int i = 0; i < midiDevices.size(); ++i)
            if (midiDevices[i]->inDevice || midiDevices[i]->outDevice)
                selectedRows.addRange(juce::Range<int>(i, i + 1));

        lastSelectedItems = selectedRows;
        updateContent();
        setSelectedRows(selectedRows, juce::dontSendNotification);
    }

//==============================================================================
    void MidiControls::handleNoteOn(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
    {
        auto m = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
        m.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
        sendToOutputs(m);
    }

    void MidiControls::handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float)
    {
        auto m = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber);
        m.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
        sendToOutputs(m);
    }

    void MidiControls::handleIncomingMidiMessage(juce::MidiInput*, const juce::MidiMessage& message)
    {
        const juce::ScopedLock sl(midiMonitorLock);
        incomingMessages.add(message);
        sendToOutputs(message);
        triggerAsyncUpdate();
    }

    void MidiControls::handleAsyncUpdate()
    {
        juce::Array<juce::MidiMessage> messages;
        {
            const juce::ScopedLock sl(midiMonitorLock);
            messages.swapWith(incomingMessages);
        }

        juce::String messageText;
        for (const auto& m : messages)
            messageText << m.getDescription() << "\n";

        midiMonitor.insertTextAtCaret(messageText);
    }

    void MidiControls::sendToOutputs(const juce::MidiMessage& msg)
    {
        // Use Tracktion Engine to process MIDI message
        tracktion_engine::MidiMessage midiMsg(msg, msg.getTimeStamp());
        if (midiMsg.isNoteOn())
            midiMsg.setVelocity(72.0f / 127.0f); // Normalize velocity (72 = specific color)

        auto rawData = midiMsg.getRawData();
        auto dataSize = midiMsg.getRawDataSize();
        juce::MidiMessage newMessage(rawData.get(), dataSize, midiMsg.getTimeStamp());

        for (const auto& midiOutput : midiOutputs)
            if (midiOutput->outDevice)
                midiOutput->outDevice->sendMessageNow(newMessage);
    }

    void MidiControls::openDevice(bool isInput, int index)
    {
        auto& list = isInput ? midiInputs : midiOutputs;
        if (isInput)
        {
            jassert(list[index]->inDevice == nullptr);
            list[index]->inDevice = juce::MidiInput::openDevice(list[index]->deviceInfo.identifier, this);
            if (!list[index]->inDevice)
            {
                DBG("MidiControls::openDevice: Failed to open input device at index " << index);
                return;
            }
            list[index]->inDevice->start();
        }
        else
        {
            jassert(list[index]->outDevice == nullptr);
            list[index]->outDevice = juce::MidiOutput::openDevice(list[index]->deviceInfo.identifier);
            if (!list[index]->outDevice)
                DBG("MidiControls::openDevice: Failed to open output device at index " << index);
        }
    }

    void MidiControls::closeDevice(bool isInput, int index)
    {
        auto& list = isInput ? midiInputs : midiOutputs;
        list[index]->stopAndReset();
    }

    bool MidiControls::hasDeviceListChanged(const juce::Array<juce::MidiDeviceInfo>& availableDevices, bool isInputDevice) const
    {
        const auto& midiDevices = isInputDevice ? midiInputs : midiOutputs;
        if (availableDevices.size() != midiDevices.size())
            return true;

        for (int i = 0; i < availableDevices.size(); ++i)
            if (availableDevices[i] != midiDevices[i]->deviceInfo)
                return true;

        return false;
    }

    MidiControls::MidiDeviceListEntry::Ptr MidiControls::findDevice(juce::MidiDeviceInfo device, bool isInputDevice) const
    {
        const auto& midiDevices = isInputDevice ? midiInputs : midiOutputs;
        for (const auto& d : midiDevices)
            if (d->deviceInfo == device)
                return d;

        return nullptr;
    }

    void MidiControls::closeUnpluggedDevices(const juce::Array<juce::MidiDeviceInfo>& currentlyPluggedInDevices, bool isInputDevice)
    {
        auto& midiDevices = isInputDevice ? midiInputs : midiOutputs;
        for (int i = midiDevices.size(); --i >= 0;)
        {
            auto& d = *midiDevices[i];
            if (!currentlyPluggedInDevices.contains(d.deviceInfo))
            {
                if (isInputDevice ? d.inDevice : d.outDevice)
                    closeDevice(isInputDevice, i);
                midiDevices.remove(i);
            }
        }
    }

    void MidiControls::updateDeviceList(bool isInputDeviceList)
    {
        auto availableDevices = isInputDeviceList ? juce::MidiInput::getAvailableDevices()
                                                  : juce::MidiOutput::getAvailableDevices();

        if (hasDeviceListChanged(availableDevices, isInputDeviceList))
        {
            auto& midiDevices = isInputDeviceList ? midiInputs : midiOutputs;
            closeUnpluggedDevices(availableDevices, isInputDeviceList);

            juce::ReferenceCountedArray<MidiDeviceListEntry> newDeviceList;
            for (const auto& newDevice : availableDevices)
            {
                auto entry = findDevice(newDevice, isInputDeviceList);
                if (!entry)
                    entry = new MidiDeviceListEntry(newDevice);
                newDeviceList.add(entry);
            }

            midiDevices = newDeviceList;

            if (auto* midiSelector = isInputDeviceList ? midiInputSelector.get() : midiOutputSelector.get())
                midiSelector->syncSelectedItemsWithDeviceList(midiDevices);
        }
    }

    void MidiControls::updateDeviceLists()
    {
        updateDeviceList(true);
        updateDeviceList(false);
    }

    void MidiControls::addLabelAndSetStyle(juce::Label& label)
    {
        label.setFont(juce::FontOptions(15.0f, juce::Font::plain));
        label.setJustificationType(juce::Justification::centredLeft);
        label.setEditable(false, false, false);
        label.setColour(juce::TextEditor::textColourId, juce::Colours::black);
        label.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x00000000));
        addAndMakeVisible(label);
    }

} // namespace trackDeck