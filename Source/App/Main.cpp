// !J! Optimized JUCE includes - only include what's necessary
#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>

/*
  ==============================================================================
   JUCE framework example code under ISC license
  ==============================================================================
*/

/*******************************************************************************
 BEGIN_JUCE_PIP_METADATA
 name:             MidiDemo
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Handles MIDI messages
 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_utils, 
                   juce_core, juce_events, juce_graphics, juce_gui_basics
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
        inDevice.reset();
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
    MidiControls()
        : midiKeyboard(keyboardState, MidiKeyboardComponent::horizontalKeyboard),
          midiInputSelector(new MidiDeviceListBox("Midi Input Selector", *this, true)),
          midiOutputSelector(new MidiDeviceListBox("Midi Output Selector", *this, false))
    {
        setupLabels();
        setupComponents();
        keyboardState.addListener(this);
        setSize(732, 520);
        updateDeviceLists();
    }

    ~MidiControls() override
    {
        midiInputs.clear();
        midiOutputs.clear();
        keyboardState.removeListener(this);
        midiInputSelector.reset();
        midiOutputSelector.reset();
    }

    void handleNoteOn(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override
    {
        MidiMessage m = MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
        m.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
        sendToOutputs(m);
    }

    void handleNoteOff(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override
    {
        MidiMessage m = MidiMessage::noteOff(midiChannel, midiNoteNumber, velocity);
        m.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
        sendToOutputs(m);
    }

    void resized() override
    {
        auto margin = 10;
        auto halfWidth = getWidth() / 2;
        midiInputLabel.setBounds(margin, margin, halfWidth - (2 * margin), 24);
        midiOutputLabel.setBounds(halfWidth + margin, margin, halfWidth - (2 * margin), 24);
        midiInputSelector->setBounds(margin, (2 * margin) + 24, halfWidth - (2 * margin), (getHeight() / 2) - ((4 * margin) + 24 + 24));
        midiOutputSelector->setBounds(halfWidth + margin, (2 * margin) + 24, halfWidth - (2 * margin), (getHeight() / 2) - ((4 * margin) + 24 + 24));
        pairButton.setBounds(margin, (getHeight() / 2) - (margin + 24), getWidth() - (2 * margin), 24);
        outgoingMidiLabel.setBounds(margin, getHeight() / 2, getWidth() - (2 * margin), 24);
        midiKeyboard.setBounds(margin, (getHeight() / 2) + (24 + margin), getWidth() - (2 * margin), 64);
        incomingMidiLabel.setBounds(margin, (getHeight() / 2) + (24 + (2 * margin) + 64), getWidth() - (2 * margin), 24);
        auto y = (getHeight() / 2) + ((2 * 24) + (3 * margin) + 64);
        midiMonitor.setBounds(margin, y, getWidth() - (2 * margin), getHeight() - y - margin);
    }

    void openDevice(bool isInput, int index)
    {
        auto& list = isInput ? midiInputs : midiOutputs;
        if (isInput)
        {
            jassert(list[index]->inDevice == nullptr);
            list[index]->inDevice = MidiInput::openDevice(list[index]->deviceInfo.identifier, this);
            if (list[index]->inDevice == nullptr)
            {
                DBG("MidiDemo::openDevice: open input device for index = " << index << " failed!");
                return;
            }
            list[index]->inDevice->start();
        }
        else
        {
            jassert(list[index]->outDevice == nullptr);
            list[index]->outDevice = MidiOutput::openDevice(list[index]->deviceInfo.identifier);
            if (list[index]->outDevice == nullptr)
                DBG("MidiDemo::openDevice: open output device for index = " << index << " failed!");
        }
    }

    void closeDevice(bool isInput, int index)
    {
        auto& list = isInput ? midiInputs : midiOutputs;
        list[index]->stopAndReset();
    }

    int getNumMidiInputs() const noexcept { return midiInputs.size(); }
    int getNumMidiOutputs() const noexcept { return midiOutputs.size(); }
    ReferenceCountedObjectPtr<MidiDeviceListEntry> getMidiDevice(int index, bool isInput) const noexcept
    {
        return isInput ? midiInputs[index] : midiOutputs[index];
    }

private:
    struct MidiDeviceListBox final : private ListBoxModel, public ListBox
    {
        MidiDeviceListBox(const String& name, MidiControls& parent_, bool isInput_)
            : ListBox(name), parent(parent_), isInput(isInput_)
        {
            setModel(this);
            setOutlineThickness(1);
            setMultipleSelectionEnabled(true);
            setClickingTogglesRowSelection(true);
        }

        int getNumRows() override { return isInput ? parent.getNumMidiInputs() : parent.getNumMidiOutputs(); }

        void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override
        {
            auto textColour = getLookAndFeel().findColour(ListBox::textColourId);
            if (rowIsSelected)
                g.fillAll(textColour.interpolatedWith(getLookAndFeel().findColour(ListBox::backgroundColourId), 0.5));
            g.setColour(textColour);
            g.setFont((float)height * 0.7f);
            auto& devices = isInput ? parent.midiInputs : parent.midiOutputs;
            if (rowNumber < devices.size())
                g.drawText(devices[rowNumber]->deviceInfo.name, 5, 0, width, height, Justification::centredLeft, true);
        }

        void selectedRowsChanged(int) override
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

        void syncSelectedItemsWithDeviceList(const ReferenceCountedArray<MidiDeviceListEntry>& midiDevices)
        {
            SparseSet<int> selectedRows;
            for (auto i = 0; i < midiDevices.size(); ++i)
                if (midiDevices[i]->inDevice != nullptr || midiDevices[i]->outDevice != nullptr)
                    selectedRows.addRange(Range<int>(i, i + 1));
            lastSelectedItems = selectedRows;
            updateContent();
            setSelectedRows(selectedRows, dontSendNotification);
        }

    private:
        MidiControls& parent;
        bool isInput;
        SparseSet<int> lastSelectedItems;
    };

    void handleIncomingMidiMessage(MidiInput*, const MidiMessage& message) override
    {
        const ScopedLock sl(midiMonitorLock);
        incomingMessages.add(message);
        sendToOutputs(message);
        triggerAsyncUpdate();
    }

    void handleAsyncUpdate() override
    {
        Array<MidiMessage> messages;
        {
            const ScopedLock sl(midiMonitorLock);
            messages.swapWith(incomingMessages);
        }
        String messageText;
        for (auto& m : messages)
            messageText << m.getDescription() << "\n";
        midiMonitor.insertTextAtCaret(messageText);
    }

    void sendToOutputs(const MidiMessage& msg)
    {
        std::vector<uint8_t> modifiedData(msg.getRawData(), msg.getRawData() + msg.getRawDataSize());
        if ((modifiedData[0] & 0xF0) == 0x90) // Note-on message
            modifiedData[2] = 72; // Fixed velocity
        MidiMessage newMessage(modifiedData.data(), msg.getRawDataSize());
        for (auto& midiOutput : midiOutputs)
            if (midiOutput->outDevice != nullptr)
                midiOutput->outDevice->sendMessageNow(newMessage);
    }

    void updateDeviceLists()
    {
        auto updateList = [&](bool isInput)
        {
            auto availableDevices = isInput ? MidiInput::getAvailableDevices() : MidiOutput::getAvailableDevices();
            auto& devices = isInput ? midiInputs : midiOutputs;
            auto* selector = isInput ? midiInputSelector.get() : midiOutputSelector.get();
            if (availableDevices.size() != devices.size() || std::any_of(availableDevices.begin(), availableDevices.end(),
                [&](auto& d) { return !devices.contains([&](auto& dev) { return dev->deviceInfo == d; }); }))
            {
                ReferenceCountedArray<MidiDeviceListEntry> newList;
                for (auto& dev : availableDevices)
                {
                    auto existing = devices.find([&](auto& d) { return d->deviceInfo == dev; });
                    newList.add(existing ? existing : new MidiDeviceListEntry(dev));
                }
                devices.swapWith(newList);
                selector->syncSelectedItemsWithDeviceList(devices);
            }
        };
        updateList(true);
        updateList(false);
    }

    void setupLabels()
    {
        auto setupLabel = [&](Label& label, const String& text)
        {
            label.setFont(FontOptions(15.0f, Font::plain));
            label.setJustificationType(Justification::centredLeft);
            label.setEditable(false, false, false);
            label.setColour(TextEditor::textColourId, Colours::black);
            label.setColour(TextEditor::backgroundColourId, Colour(0x00000000));
            label.setText(text, dontSendNotification);
            addAndMakeVisible(label);
        };
        setupLabel(midiInputLabel, "MIDI Input:");
        setupLabel(midiOutputLabel, "MIDI Output:");
        setupLabel(incomingMidiLabel, "Received MIDI messages:");
        setupLabel(outgoingMidiLabel, "Play the keyboard to send MIDI messages...");
    }

    void setupComponents()
    {
        midiKeyboard.setName("MIDI Keyboard");
        addAndMakeVisible(midiKeyboard);
        
        midiMonitor.setMultiLine(true);
        midiMonitor.setReadOnly(true);
        midiMonitor.setScrollbarsShown(true);
        midiMonitor.setCaretVisible(false);
        midiMonitor.setPopupMenuEnabled(false);
        addAndMakeVisible(midiMonitor);

        pairButton.setEnabled(BluetoothMidiDevicePairingDialogue::isAvailable());
        addAndMakeVisible(pairButton);
        pairButton.onClick = []
        {
            RuntimePermissions::request(RuntimePermissions::bluetoothMidi,
                [](bool granted) { if (granted) BluetoothMidiDevicePairingDialogue::open(); });
        };

        addAndMakeVisible(midiInputSelector.get());
        addAndMakeVisible(midiOutputSelector.get());
    }

    Label midiInputLabel, midiOutputLabel, incomingMidiLabel, outgoingMidiLabel;
    MidiKeyboardState keyboardState;
    MidiKeyboardComponent midiKeyboard;
    TextEditor midiMonitor{"MIDI Monitor"};
    TextButton pairButton{"MIDI Bluetooth devices..."};
    ReferenceCountedArray<MidiDeviceListEntry> midiInputs, midiOutputs;
    std::unique_ptr<MidiDeviceListBox> midiInputSelector, midiOutputSelector;
    CriticalSection midiMonitorLock;
    Array<MidiMessage> incomingMessages;
    MidiDeviceListConnection connection = MidiDeviceListConnection::make([this] { updateDeviceLists(); });

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiControls)
};

//==============================================================================
class StepperDialog : public Component, public Button::Listener
{
public:
    StepperDialog()
    {
        addAndMakeVisible(lMidiControls);
        launchStopButton.setButtonText("Launch/Stop");
        launchStopButton.addListener(this);
        addAndMakeVisible(launchStopButton);
        quitButton.setButtonText("Quit");
        quitButton.addListener(this);
        addAndMakeVisible(quitButton);
        loadSettings();
        setSize(720, 800);
    }

    ~StepperDialog() override
    {
        launchStopButton.removeListener(this);
        quitButton.removeListener(this);
        saveSettings();
    }

    void paint(Graphics& g) override { g.fillAll(Colours::white); }

    void resized() override
    {
        auto area = getLocalBounds().reduced(10);
        lMidiControls.setBounds(area.removeFromTop(700));
        launchStopButton.setBounds(area.removeFromTop(40));
        quitButton.setBounds(area.removeFromTop(40).withTrimmedTop(10));
    }

    void buttonClicked(Button* button) override
    {
        if (button == &launchStopButton)
        {
            // TODO: Implement launch/stop functionality
        }
        else if (button == &quitButton)
            JUCEApplication::getInstance()->systemRequestedQuit();
    }

private:
    TextButton launchStopButton, quitButton;
    MidiControls lMidiControls;
    ApplicationProperties appProperties;

    void loadSettings()
    {
        PropertiesFile::Options opt;
        opt.applicationName = "apcStepper";
        opt.commonToAllUsers = false;
        opt.filenameSuffix = ".props";
        opt.storageFormat = PropertiesFile::storeAsXML;
        opt.osxLibrarySubFolder = "Application Support";
        appProperties.setStorageParameters(opt);
        
        auto lastPosition = appProperties.getUserSettings()->getValue("lastWindowBounds");
        if (!lastPosition.isEmpty())
            setBounds(Rectangle<int>::fromString(lastPosition));
        else
            centreWithSize(720, 800);
    }

    void saveSettings()
    {
        appProperties.getUserSettings()->setValue("lastWindowBounds", getBounds().toString());
        appProperties.saveIfNeeded();
    }
};

//==============================================================================
class StepperApplication : public JUCEApplication
{
public:
    const String getApplicationName() override { return "apcStepper"; }
    const String getApplicationVersion() override { return "1.0.0"; }

    void initialise(const String&) override
    {
        mainWindow = std::make_unique<MainWindow>("Stepper", new StepperDialog(), *this);
    }

    void shutdown() override { mainWindow.reset(); }

private:
    class MainWindow : public DocumentWindow
    {
    public:
        MainWindow(const String& name, Component* c, JUCEApplication& app)
            : DocumentWindow(name, Desktop::getInstance().getDefaultLookAndFeel()
                           .findColour(ResizableWindow::backgroundColourId), allButtons),
              appRef(app)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(c, true);
            centreWithSize(getWidth(), getHeight());
            setVisible(true);
        }

        void closeButtonPressed() override { appRef.systemRequestedQuit(); }

    private:
        JUCEApplication& appRef;
    };

    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(StepperApplication)
