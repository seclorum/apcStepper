// !J! Just include all the JUCE things.  Makes it easier to navigate... but slower to build
#include <juce_analytics/juce_analytics.h>
#include <juce_animation/juce_animation.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_box2d/juce_box2d.h>
#include <juce_core/juce_core.h>
#include <juce_cryptography/juce_cryptography.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_javascript/juce_javascript.h>
#include <juce_midi_ci/juce_midi_ci.h>
#include <juce_opengl/juce_opengl.h>
#include <juce_osc/juce_osc.h>
#include <juce_product_unlocking/juce_product_unlocking.h>
#include <juce_video/juce_video.h>




/*
  ==============================================================================

   This file is part of the JUCE framework examples.
   Copyright (c) Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   to use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
   REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
   AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
   INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
   OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
   PERFORMANCE OF THIS SOFTWARE.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             MidiDemo
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Handles incoming and outcoming midi messages.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_processors, juce_audio_utils, juce_core,
                   juce_data_structures, juce_events, juce_graphics,
                   juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2022, linux_make, androidstudio, xcode_iphone

 moduleFlags:      JUCE_STRICT_REFCOUNTEDPOINTER=1

 type:             Component
 mainClass:        MidiDemo

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

using namespace juce;

//==============================================================================
struct MidiDeviceListEntry final : ReferenceCountedObject
{
    explicit MidiDeviceListEntry (MidiDeviceInfo info) : deviceInfo (info) {}

    MidiDeviceInfo deviceInfo;
    std::unique_ptr<MidiInput> inDevice;
    std::unique_ptr<MidiOutput> outDevice;

    using Ptr = ReferenceCountedObjectPtr<MidiDeviceListEntry>;

    void stopAndReset()
    {
        if (inDevice != nullptr)
            inDevice->stop();

        inDevice .reset();
        outDevice.reset();
    }
};


//==============================================================================
class MidiControls final : public Component,
                       private MidiKeyboardState::Listener,
                       private MidiInputCallback,
                       private AsyncUpdater
{
public:
    //==============================================================================
    MidiControls()
        : midiKeyboard       (keyboardState, MidiKeyboardComponent::horizontalKeyboard),
          midiInputSelector  (new MidiDeviceListBox ("Midi Input Selector",  *this, true)),
          midiOutputSelector (new MidiDeviceListBox ("Midi Output Selector", *this, false))
    {
        addLabelAndSetStyle (midiInputLabel);
        addLabelAndSetStyle (midiOutputLabel);
        addLabelAndSetStyle (incomingMidiLabel);
        addLabelAndSetStyle (outgoingMidiLabel);

        midiKeyboard.setName ("MIDI Keyboard");
        addAndMakeVisible (midiKeyboard);

        midiMonitor.setMultiLine (true);
        midiMonitor.setReturnKeyStartsNewLine (false);
        midiMonitor.setReadOnly (true);
        midiMonitor.setScrollbarsShown (true);
        midiMonitor.setCaretVisible (false);
        midiMonitor.setPopupMenuEnabled (false);
        midiMonitor.setText ({});
        addAndMakeVisible (midiMonitor);

        if (! BluetoothMidiDevicePairingDialogue::isAvailable())
            pairButton.setEnabled (false);

        addAndMakeVisible (pairButton);
        pairButton.onClick = []
        {
            RuntimePermissions::request (RuntimePermissions::bluetoothMidi,
                                         [] (bool wasGranted)
                                         {
                                             if (wasGranted)
                                                 BluetoothMidiDevicePairingDialogue::open();
                                         });
        };
        keyboardState.addListener (this);

        addAndMakeVisible (midiInputSelector .get());
        addAndMakeVisible (midiOutputSelector.get());

        setSize (732, 520);

        updateDeviceLists();
    }

    ~MidiControls() override
    {
        midiInputs .clear();
        midiOutputs.clear();
        keyboardState.removeListener (this);

        midiInputSelector .reset();
        midiOutputSelector.reset();
    }

    //==============================================================================
    void handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override
    {
        MidiMessage m (MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity));
        m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
        sendToOutputs (m);

    }

    void handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override
    {
        MidiMessage m (MidiMessage::noteOff (midiChannel, midiNoteNumber, velocity));
        m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
        sendToOutputs (m);
    }

    void paint (Graphics&) override {}

    void resized() override
    {
        auto margin = 10;

        midiInputLabel.setBounds (margin, margin,
                                  (getWidth() / 2) - (2 * margin), 24);

        midiOutputLabel.setBounds ((getWidth() / 2) + margin, margin,
                                   (getWidth() / 2) - (2 * margin), 24);

        midiInputSelector->setBounds (margin, (2 * margin) + 24,
                                      (getWidth() / 2) - (2 * margin),
                                      (getHeight() / 2) - ((4 * margin) + 24 + 24));

        midiOutputSelector->setBounds ((getWidth() / 2) + margin, (2 * margin) + 24,
                                       (getWidth() / 2) - (2 * margin),
                                       (getHeight() / 2) - ((4 * margin) + 24 + 24));

        pairButton.setBounds (margin, (getHeight() / 2) - (margin + 24),
                              getWidth() - (2 * margin), 24);

        outgoingMidiLabel.setBounds (margin, getHeight() / 2, getWidth() - (2 * margin), 24);
        midiKeyboard.setBounds (margin, (getHeight() / 2) + (24 + margin), getWidth() - (2 * margin), 64);

        incomingMidiLabel.setBounds (margin, (getHeight() / 2) + (24 + (2 * margin) + 64),
                                     getWidth() - (2 * margin), 24);

        auto y = (getHeight() / 2) + ((2 * 24) + (3 * margin) + 64);
        midiMonitor.setBounds (margin, y,
                               getWidth() - (2 * margin), getHeight() - y - margin);
    }

    void openDevice (bool isInput, int index)
    {
        if (isInput)
        {
            jassert (midiInputs[index]->inDevice.get() == nullptr);
            midiInputs[index]->inDevice = MidiInput::openDevice (midiInputs[index]->deviceInfo.identifier, this);

            if (midiInputs[index]->inDevice.get() == nullptr)
            {
                DBG ("MidiDemo::openDevice: open input device for index = " << index << " failed!");
                return;
            }

            midiInputs[index]->inDevice->start();
        }
        else
        {
            jassert (midiOutputs[index]->outDevice.get() == nullptr);
            midiOutputs[index]->outDevice = MidiOutput::openDevice (midiOutputs[index]->deviceInfo.identifier);

            if (midiOutputs[index]->outDevice.get() == nullptr)
            {
                DBG ("MidiDemo::openDevice: open output device for index = " << index << " failed!");
            }
        }
    }

    void closeDevice (bool isInput, int index)
    {
        auto& list = isInput ? midiInputs : midiOutputs;
        list[index]->stopAndReset();
    }

    int getNumMidiInputs() const noexcept
    {
        return midiInputs.size();
    }

    int getNumMidiOutputs() const noexcept
    {
        return midiOutputs.size();
    }

    ReferenceCountedObjectPtr<MidiDeviceListEntry> getMidiDevice (int index, bool isInput) const noexcept
    {
        return isInput ? midiInputs[index] : midiOutputs[index];
    }

private:
    //==============================================================================
    struct MidiDeviceListBox final : private ListBoxModel,
                                     public ListBox
    {
        MidiDeviceListBox (const String& name,
                           MidiControls& contentComponent,
                           bool isInputDeviceList)
            : ListBox (name),
              parent (contentComponent),
              isInput (isInputDeviceList)
        {
            setModel (this);
            setOutlineThickness (1);
            setMultipleSelectionEnabled (true);
            setClickingTogglesRowSelection (true);
        }

        //==============================================================================
        int getNumRows() override
        {
            return isInput ? parent.getNumMidiInputs()
                           : parent.getNumMidiOutputs();
        }

        void paintListBoxItem (int rowNumber, Graphics& g,
                               int width, int height, bool rowIsSelected) override
        {
            auto textColour = getLookAndFeel().findColour (ListBox::textColourId);

            if (rowIsSelected)
                g.fillAll (textColour.interpolatedWith (getLookAndFeel().findColour (ListBox::backgroundColourId), 0.5));


            g.setColour (textColour);
            g.setFont ((float) height * 0.7f);

            if (isInput)
            {
                if (rowNumber < parent.getNumMidiInputs())
                    g.drawText (parent.getMidiDevice (rowNumber, true)->deviceInfo.name,
                                5, 0, width, height,
                                Justification::centredLeft, true);
            }
            else
            {
                if (rowNumber < parent.getNumMidiOutputs())
                    g.drawText (parent.getMidiDevice (rowNumber, false)->deviceInfo.name,
                                5, 0, width, height,
                                Justification::centredLeft, true);
            }
        }

        //==============================================================================
        void selectedRowsChanged (int) override
        {
            auto newSelectedItems = getSelectedRows();
            if (newSelectedItems != lastSelectedItems)
            {
                for (auto i = 0; i < lastSelectedItems.size(); ++i)
                {
                    if (! newSelectedItems.contains (lastSelectedItems[i]))
                        parent.closeDevice (isInput, lastSelectedItems[i]);
                }

                for (auto i = 0; i < newSelectedItems.size(); ++i)
                {
                    if (! lastSelectedItems.contains (newSelectedItems[i]))
                        parent.openDevice (isInput, newSelectedItems[i]);
                }

                lastSelectedItems = newSelectedItems;
            }
        }

        //==============================================================================
        void syncSelectedItemsWithDeviceList (const ReferenceCountedArray<MidiDeviceListEntry>& midiDevices)
        {
            SparseSet<int> selectedRows;
            for (auto i = 0; i < midiDevices.size(); ++i)
                if (midiDevices[i]->inDevice != nullptr || midiDevices[i]->outDevice != nullptr)
                    selectedRows.addRange (Range<int> (i, i + 1));

            lastSelectedItems = selectedRows;
            updateContent();
            setSelectedRows (selectedRows, dontSendNotification);
        }

    private:
        //==============================================================================
        MidiControls& parent;
        bool isInput;
        SparseSet<int> lastSelectedItems;
    };

    //==============================================================================
    void handleIncomingMidiMessage (MidiInput* /*source*/, const MidiMessage& message) override
    {
        // This is called on the MIDI thread
        const ScopedLock sl (midiMonitorLock);
        incomingMessages.add (message);
        sendToOutputs(message);
        triggerAsyncUpdate();
    }

    void handleAsyncUpdate() override
    {
        // This is called on the message loop
        Array<MidiMessage> messages;

        {
            const ScopedLock sl (midiMonitorLock);
            messages.swapWith (incomingMessages);
        }

        String messageText;

        for (auto& m : messages)
            messageText << m.getDescription() << "\n";

        midiMonitor.insertTextAtCaret (messageText);
    }

    void sendToOutputs (const MidiMessage& msg)
    {

            // Get the raw data and sizelib
            const uint8_t* rawData = msg.getRawData();
            int dataSize = msg.getRawDataSize();

            // Create a modifiable copy of the original data
            std::vector<uint8_t> modifiedData(rawData, rawData + dataSize);
            // Example: Modify the velocity (3rd byte) if it's a note-on message
            if ((modifiedData[0] & 0xF0) == 0x90) // Check if it's a note-on message
            {
                //modifiedData[1] = 26;
                modifiedData[2] = 72; ///0 = off 1=dunkelblau 2=hellblaus 3 =blau 4=lila turkis 5/6 = red 8=white 9 = yellow
            }

            // Create a new MidiMessage from the modified data
            juce::MidiMessage newMessage(modifiedData.data(), dataSize);

            // Debug the new message

        for (auto midiOutput : midiOutputs)
            if (midiOutput->outDevice != nullptr)
                midiOutput->outDevice->sendMessageNow (newMessage);
    }

    //==============================================================================
    bool hasDeviceListChanged (const Array<MidiDeviceInfo>& availableDevices, bool isInputDevice)
    {
        ReferenceCountedArray<MidiDeviceListEntry>& midiDevices = isInputDevice ? midiInputs
                                                                                : midiOutputs;

        if (availableDevices.size() != midiDevices.size())
            return true;

        for (auto i = 0; i < availableDevices.size(); ++i)
            if (availableDevices[i] != midiDevices[i]->deviceInfo)
                return true;

        return false;
    }

    ReferenceCountedObjectPtr<MidiDeviceListEntry> findDevice (MidiDeviceInfo device, bool isInputDevice) const
    {
        const ReferenceCountedArray<MidiDeviceListEntry>& midiDevices = isInputDevice ? midiInputs
                                                                                      : midiOutputs;

        for (auto& d : midiDevices)
            if (d->deviceInfo == device)
                return d;

        return nullptr;
    }

    void closeUnpluggedDevices (const Array<MidiDeviceInfo>& currentlyPluggedInDevices, bool isInputDevice)
    {
        ReferenceCountedArray<MidiDeviceListEntry>& midiDevices = isInputDevice ? midiInputs
                                                                                : midiOutputs;

        for (auto i = midiDevices.size(); --i >= 0;)
        {
            auto& d = *midiDevices[i];

            if (! currentlyPluggedInDevices.contains (d.deviceInfo))
            {
                if (isInputDevice ? d.inDevice .get() != nullptr
                                  : d.outDevice.get() != nullptr)
                    closeDevice (isInputDevice, i);

                midiDevices.remove (i);
            }
        }
    }

    void updateDeviceList (bool isInputDeviceList)
    {
        auto availableDevices = isInputDeviceList ? MidiInput::getAvailableDevices()
                                                  : MidiOutput::getAvailableDevices();

        if (hasDeviceListChanged (availableDevices, isInputDeviceList))
        {
            ReferenceCountedArray<MidiDeviceListEntry>& midiDevices
                = isInputDeviceList ? midiInputs : midiOutputs;

            closeUnpluggedDevices (availableDevices, isInputDeviceList);

            ReferenceCountedArray<MidiDeviceListEntry> newDeviceList;

            // add all currently plugged-in devices to the device list
            for (auto& newDevice : availableDevices)
            {
                MidiDeviceListEntry::Ptr entry = findDevice (newDevice, isInputDeviceList);

                if (entry == nullptr)
                    entry = new MidiDeviceListEntry (newDevice);

                newDeviceList.add (entry);
            }

            // actually update the device list
            midiDevices = newDeviceList;

            // update the selection status of the combo-box
            if (auto* midiSelector = isInputDeviceList ? midiInputSelector.get() : midiOutputSelector.get())
                midiSelector->syncSelectedItemsWithDeviceList (midiDevices);
        }
    }

    //==============================================================================
    void addLabelAndSetStyle (Label& label)
    {
        label.setFont (FontOptions (15.00f, Font::plain));
        label.setJustificationType (Justification::centredLeft);
        label.setEditable (false, false, false);
        label.setColour (TextEditor::textColourId, Colours::black);
        label.setColour (TextEditor::backgroundColourId, Colour (0x00000000));

        addAndMakeVisible (label);
    }

    void updateDeviceLists()
    {
        for (const auto isInput : { true, false })
            updateDeviceList (isInput);
    }

    //==============================================================================
    Label midiInputLabel    { "Midi Input Label",  "MIDI Input:" };
    Label midiOutputLabel   { "Midi Output Label", "MIDI Output:" };
    Label incomingMidiLabel { "Incoming Midi Label", "Received MIDI messages:" };
    Label outgoingMidiLabel { "Outgoing Midi Label", "Play the keyboard to send MIDI messages..." };
    MidiKeyboardState keyboardState;
    MidiKeyboardComponent midiKeyboard;
    TextEditor midiMonitor  { "MIDI Monitor" };
    TextButton pairButton   { "MIDI Bluetooth devices..." };

    ReferenceCountedArray<MidiDeviceListEntry> midiInputs, midiOutputs;
    std::unique_ptr<MidiDeviceListBox> midiInputSelector, midiOutputSelector;

    CriticalSection midiMonitorLock;
    Array<MidiMessage> incomingMessages;

    MidiDeviceListConnection connection = MidiDeviceListConnection::make ([this]
    {
        updateDeviceLists();
    });

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiControls)
};




class StepperDialog : public juce::Component, public juce::Button::Listener
{
public:
    StepperDialog()
    {
        addAndMakeVisible(lMidiControls);

        // Initialize buttons and labels
        addAndMakeVisible(launchStopButton);
        launchStopButton.setButtonText("Launch/Stop");
        launchStopButton.addListener(this);

        addAndMakeVisible(quitButton);
        quitButton.setButtonText("Quit");
        quitButton.addListener(this);


        // Remember last position and settings
        loadSettings();
    }

    ~StepperDialog() override
    {
        // Remove listeners when the component is destroyed to avoid dangling references
        launchStopButton.removeListener(this);
        quitButton.removeListener(this);

        saveSettings();
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::white);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(10);

        lMidiControls.setBounds(area.removeFromTop(700));
        launchStopButton.setBounds(area.removeFromTop(40));
        quitButton.setBounds(area.removeFromTop(40).withTrimmedTop(10));
    }

    void buttonClicked(juce::Button* button) override
    {
        if (button == &launchStopButton)
        {
            // Handle launch/stop action
        }
        else if (button == &quitButton)
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    }

private:
    juce::TextButton launchStopButton;
    juce::TextButton quitButton;

    MidiControls lMidiControls;

    juce::ApplicationProperties appProperties;

    void loadSettings()
    {
        juce::PropertiesFile::Options opt;

        opt.applicationName = "apcStepper";
        opt.commonToAllUsers = false;
        opt.doNotSave = false;
        opt.filenameSuffix = ".props";
        opt.ignoreCaseOfKeyNames = false;
        opt.storageFormat = juce::PropertiesFile::StorageFormat::storeAsXML;
        opt.osxLibrarySubFolder = "Application Support";

        appProperties.setStorageParameters(opt);

        auto lastPosition = appProperties.getUserSettings()->getValue("lastWindowBounds", "");

        if (!lastPosition.isEmpty())
        {
            auto bounds = juce::Rectangle<int>::fromString(lastPosition);
            setBounds(bounds);
        }
        else
        {
            centreWithSize(720, 800);
        }
    }

    void saveSettings()
    {
        appProperties.getUserSettings()->setValue("lastWindowBounds", getBounds().toString());
        appProperties.saveIfNeeded();
        appProperties.closeFiles();
    }
};

class StepperApplication : public juce::JUCEApplication
{
public:

    const juce::String getApplicationName() override { return "apcStepper"; }
    const juce::String getApplicationVersion() override  { return "1.0.0"; }

    void initialise(const juce::String&) override
    {
        mainWindow = std::make_unique<MainWindow>("Stepper", new StepperDialog(), *this);

    }

    void shutdown() override
    {
        mainWindow.reset();  // Properly reset the main window to ensure memory is freed
    }

private:
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(const juce::String& name, juce::Component* c, JUCEApplication& app)
                : DocumentWindow(name,
                                 juce::Desktop::getInstance().getDefaultLookAndFeel()
                                         .findColour(ResizableWindow::backgroundColourId),
                                 allButtons),
                  appRef(app)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(c, true);
            centreWithSize(getWidth(), getHeight());
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            appRef.systemRequestedQuit();
        }

    private:
        JUCEApplication& appRef;
    };

    std::unique_ptr<MainWindow> mainWindow;
};

// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION(StepperApplication)
