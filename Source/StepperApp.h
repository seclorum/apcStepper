#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

class StepperDialog : public juce::Component, 
                       public juce::Button::Listener,
                       public juce::MouseListener
{
public:
    StepperDialog()
    {
        // Initialize buttons and labels
        addAndMakeVisible(launchStopButton);
        launchStopButton.setButtonText("Launch/Stop");
        launchStopButton.addListener(this);

        addAndMakeVisible(quitButton);
        quitButton.setButtonText("Quit");
        quitButton.addListener(this);

        addAndMakeVisible(homepageLink);
        homepageLink.setText("Homepage", juce::NotificationType::dontSendNotification);
        homepageLink.setColour(juce::Label::textColourId, juce::Colours::blue);
        homepageLink.addListener(this);

        addAndMakeVisible(manualLink);
        manualLink.setText("Manual", juce::NotificationType::dontSendNotification);
        manualLink.setColour(juce::Label::textColourId, juce::Colours::blue);
        manualLink.addListener(this);

        addAndMakeVisible(supportLink);
        supportLink.setText("Support", juce::NotificationType::dontSendNotification);
        supportLink.setColour(juce::Label::textColourId, juce::Colours::blue);
        supportLink.addListener(this);

        // Remember last position and settings
        loadSettings();
    }

    ~StepperDialog() override
    {
        saveSettings();
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::white);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(10);

        launchStopButton.setBounds(area.removeFromTop(40));
        quitButton.setBounds(area.removeFromTop(40).withTrimmedTop(10));

        homepageLink.setBounds(area.removeFromTop(20).withTrimmedTop(10));
        manualLink.setBounds(area.removeFromTop(20).withTrimmedTop(5));
        supportLink.setBounds(area.removeFromTop(20).withTrimmedTop(5));
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

    void mouseUp(const juce::MouseEvent& event) override
    {
        if (event.eventComponent == &homepageLink)
        {
            juce::URL("http://gamehomepage.com").launchInDefaultBrowser();
        }
        else if (event.eventComponent == &manualLink)
        {
            juce::URL("http://gamemanual.com").launchInDefaultBrowser();
        }
        else if (event.eventComponent == &supportLink)
        {
            juce::URL("mailto:support@game.com").launchInDefaultBrowser();
        }
    }

private:
    juce::TextButton launchStopButton;
    juce::TextButton quitButton;
    juce::Label homepageLink;
    juce::Label manualLink;
    juce::Label supportLink;

    void loadSettings()
    {
        auto* appProperties = juce::ApplicationProperties::getInstance();
        auto lastPosition = appProperties->getUserSettings()->getValue("lastWindowBounds", "");

        if (!lastPosition.isEmpty())
        {
            auto bounds = juce::Rectangle<int>::fromString(lastPosition);
            setBounds(bounds);
        }
        else
        {
            centreWithSize(400, 300);
        }
    }

    void saveSettings()
    {
        auto* appProperties = juce::ApplicationProperties::getInstance();
        appProperties->getUserSettings()->setValue("lastWindowBounds", getBounds().toString());
    }
};

class StepperApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "Stepper"; }
    const juce::String getApplicationVersion() override { return "1.0.0"; }

    void initialise(const juce::String&) override
    {
        mainWindow = std::make_unique<MainWindow>("Stepper", new StepperDialog(), *this);
    }

    void shutdown() override
    {
        mainWindow = nullptr;
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

