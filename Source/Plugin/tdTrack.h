class tdTrack : public juce::AudioProcessorEditor, 
                private juce::Timer, 
                private juce::Button::Listener,private juce::AudioProcessorParameter::Listener




{
public:
    static constexpr int padding = 6.0f;

    tdTrack(trackDeckMainProcessor& p, const int i)
        : AudioProcessorEditor(&p), processor(p), instrumentNumber(i)
    {
        static constexpr int steps = 16;

        // Initialize step squares
        for (int col = 0; col < steps; ++col) {
            auto square = std::make_unique<apcToggleParameterButton>(
                "s" + processor.addLeadingZeros(col) + "t" + processor.addLeadingZeros(instrumentNumber),
                col, instrumentNumber, juce::Colours::aliceblue, processor);
            addAndMakeVisible(*square);
            squares.add(std::move(square));
            APCLOG("step_" + std::to_string(col) + "_track_" + std::to_string(instrumentNumber));
        }

        // Initialize mute button
        muteButton = std::make_unique<apcToggleButton>(juce::Colours::darkgrey, juce::Colours::darkred);
        addAndMakeVisible(*muteButton);
        muteButton->setClickingTogglesState(true);
        muteButton->addListener(this);

        // Attach mute button to the track's mute parameter


        // Attach to currentTrack parameter for track selection
        current_attachment = processor.getParameters().getParameter("currentTrack");
        if (current_attachment) {
            current_attachment->addListener(this);
        }

        startTimer(25);
        APCLOG("tdTrack initialized for track " + std::to_string(instrumentNumber));
    }

    ~tdTrack() override 
    {
        if (current_attachment)
            current_attachment->removeListener(this);
    }

    void buttonClicked(juce::Button* button) override 
    {
        if (button == muteButton.get()) {
            // The muteAttachment handles the parameter update automatically
            // Optionally, set currentTrack to this track
            if (auto* param = processor.getParameters().getParameter("currentTrack")) {
                param->setValueNotifyingHost(static_cast<float>(instrumentNumber));
            }
        }
    }

    void parameterValueChanged(int parameterIndex, float newValue) override
    {
        if (current_attachment && current_attachment->getParameterIndex() == parameterIndex) {
            // Highlight this track if it's the current track
            bool isCurrentTrack = (static_cast<int>(newValue) == instrumentNumber);
            muteButton->setAlpha(isCurrentTrack ? 1.0f : 0.5f); // Example: Highlight current track
        }
    }

    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override
    {
        // Empty if not needed
    }

    void timerCallback() override 
    {
        for (int i = 0; i < processor.numSteps; ++i) {
            std::string parameterID = "c" + processor.addLeadingZeros(i);
            if (auto* param = dynamic_cast<juce::AudioParameterBool*>(processor.getParameters().getParameter(parameterID))) {
                param->setValueNotifyingHost((i == processor.currentMIDIStep) ? 1.0f : 0.0f);
            } else {
                DBG("Parameter " + parameterID + " not found or not a bool!");
            }
        }
    }

    void resized() override 
    {
        auto bounds = getLocalBounds();
        float squareSize = bounds.getHeight() - padding;

        juce::FlexBox rowFlexBox;
        rowFlexBox.flexDirection = juce::FlexBox::Direction::row;
        rowFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        rowFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;

        for (auto& square : squares) {
            rowFlexBox.items.add(juce::FlexItem(*square).withFlex(1).withMargin(6));
        }

        juce::FlexBox trackFlexBox;
        trackFlexBox.flexDirection = juce::FlexBox::Direction::row;
        trackFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        trackFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;

        trackFlexBox.items.add(juce::FlexItem(rowFlexBox).withFlex(12));
        trackFlexBox.items.add(juce::FlexItem(*muteButton).withFlex(1));

        trackFlexBox.performLayout(bounds.toFloat());
    }

private:
    trackDeckMainProcessor& processor;
    juce::OwnedArray<apcToggleParameterButton> squares;
    std::unique_ptr<apcToggleButton> muteButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> muteAttachment;
    juce::RangedAudioParameter* current_attachment = nullptr;
    const int instrumentNumber;

    // Removed unused members
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(tdTrack)
};