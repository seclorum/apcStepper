// Created by Tom Peak Walcher on 19.04.25.
//

#include "Common.h"
#include "apcLog.h"
#include "apcToggleButton.h"
#include "apcToggleParameterButton.h"

class trackDeckMainProcessor;

class CustomSlider : public juce::Slider {
public:
    CustomSlider() {
        setInterceptsMouseClicks(true, true);
        setEnabled(true);
        setSliderSnapsToMousePosition(true);
        setScrollWheelEnabled(true);
    }

    void mouseDown(const juce::MouseEvent &event) override {
        DBG("CustomSlider MouseDown! Value: " + juce::String(getValue()));
        juce::Slider::mouseDown(event);
    }

    void mouseDrag(const juce::MouseEvent &event) override {
        DBG("CustomSlider MouseDrag! Value: " + juce::String(getValue()));
        juce::Slider::mouseDrag(event);
    }
};

class tdTrackController : public juce::AudioProcessorEditor, private juce::Timer {
public:
    static constexpr float padding = 6.0f;

    tdTrackController(trackDeckMainProcessor &p, const int s)
        : AudioProcessorEditor(&p), processor(p), stepNumber(s),
          customSliderLookAndFeel(std::make_unique<CustomSliderLookAndFeel>(this)) {
        setInterceptsMouseClicks(true, true);
        //onMouseDown = [] { DBG("Editor MouseDown!"); };

        slider1 = std::make_unique<CustomSlider>();
        slider2 = std::make_unique<CustomSlider>();

        configureSlider(slider1.get());
        addAndMakeVisible(slider1.get());
        configureSlider(slider2.get());
        addAndMakeVisible(slider2.get());

        APCLOG("trackDeckTrack initialized...");
        startTimer(25);
    }

    ~tdTrackController() override {
        if (slider1) slider1->setLookAndFeel(nullptr);
        if (slider2) slider2->setLookAndFeel(nullptr);
    }

    void resized() override {
        auto bounds = getLocalBounds();

        DBG("Parent bounds: " + bounds.toString());

        juce::FlexBox sliderFlexBox;
        sliderFlexBox.flexDirection = juce::FlexBox::Direction::row;
        sliderFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        sliderFlexBox.alignItems = juce::FlexBox::AlignItems::center;

        float sliderWidth = bounds.getWidth() * 0.9f;
        float sliderHeight = bounds.getHeight() * 0.9f;

        sliderFlexBox.items.add(
            juce::FlexItem(*slider1).withWidth(sliderWidth).withHeight(sliderHeight).withFlex(1).withMargin(
                juce::FlexItem::Margin(2)));
        sliderFlexBox.items.add(
            juce::FlexItem(*slider2).withWidth(sliderWidth).withHeight(sliderHeight).withFlex(1).withMargin(
                juce::FlexItem::Margin(2)));

        sliderFlexBox.performLayout(bounds.toFloat());

        DBG("Slider1 bounds: " + slider1->getBounds().toString());
        DBG("Slider2 bounds: " + slider2->getBounds().toString());
    }

    void configureSlider(juce::Slider *slider) {
        slider->setSliderStyle(juce::Slider::LinearVertical);
        slider->setRange(0.0, 128.0, 1.0);
        slider->setValue(64.0);
        slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider->setSliderSnapsToMousePosition(true);
        slider->setScrollWheelEnabled(true);
        slider->setEnabled(true);
        slider->setBounds(getLocalBounds());
        slider->setInterceptsMouseClicks(true, true);
        slider->setLookAndFeel(customSliderLookAndFeel.get());
        slider->onValueChange = [this, slider] {
            //DBG("Slider value changed to: " + juce::String(slider->getValue()));
            juce::LookAndFeel *currentLF = &slider->getLookAndFeel();
            //DBG("onValueChange - LookAndFeel for slider " + juce::String(slider == slider1.get() ? 1 : 2) + " is: " + juce::String(reinterpret_cast<intptr_t>(currentLF), 16));
            if (auto *lf = dynamic_cast<CustomSliderLookAndFeel *>(currentLF)) {
                lf->valueChanged(slider);
            } else {
                //DBG("onValueChange - dynamic_cast failed for slider " + juce::String(slider == slider1.get() ? 1 : 2));
                slider->repaint(); // Fallback
            }
        };
    }

    void timerCallback() override {
        for (int i = 0; i < processor.numSteps; ++i) {
            std::string parameterID = "c" + processor.addLeadingZeros(i);
            if (auto *param = dynamic_cast<juce::AudioParameterBool *>(processor.parameters.
                getParameter(parameterID))) {
                if (i == processor.currentMIDIStep) param->setValueNotifyingHost(1.0f);
                else param->setValueNotifyingHost(0.0f);
            } else {
                DBG("Parameter " + parameterID + " not found or not a bool!");
            }
        }
    }

    void setColumnforStep(int step) {
        if (step == stepNumber) {
            setColour(juce::Label::backgroundColourId, juce::Colours::orange);
        } else {
            setColour(juce::Label::backgroundColourId, juce::Colours::white);
        }
    }

private:
    class CustomSliderLookAndFeel : public juce::LookAndFeel_V4 {
    public:
        CustomSliderLookAndFeel(tdTrackController *parent) : owner(parent) {
        }

        void drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height,
                              float sliderPos, float minSliderPos, float maxSliderPos,
                              const juce::Slider::SliderStyle style, juce::Slider &slider) override {
            float actualMin = slider.getRange().getStart();
            float actualMax = slider.getRange().getEnd();
            auto actualRange = slider.getRange();

            g.setColour(juce::Colours::black);
            g.fillRect(x, y, width, height);

            if (actualRange.getLength() > 0) {
                float normalizedPos = (slider.getValue() - actualMin) / actualRange.getLength();
                int fillHeight = juce::roundToInt(height * normalizedPos);;

                juce::ColourGradient gradient;
                if (&slider == owner->slider1.get()) {
                    gradient = juce::ColourGradient(
                        juce::Colour(0xff8b0000), (float) x, (float) (y + height),
                        juce::Colour(0xffff4500), (float) x, (float) y, false);
                } else {
                    gradient = juce::ColourGradient(
                        juce::Colour(0xff00008b), (float) x, (float) (y + height),
                        juce::Colour(0xffadd8e6), (float) x, (float) y, false);
                }
                g.setGradientFill(gradient);
                g.fillRect(x, y + height - fillHeight, width, fillHeight);

                g.setColour(juce::Colours::white);
                int handleHeight = 2;
                int handleY = y + height - fillHeight - handleHeight / 2;
                handleY = juce::jlimit(y, y + height - handleHeight, handleY);
                g.fillRect(x, handleY, width, handleHeight);
            } else {
                DBG("Error: Slider range is zero in drawLinearSlider!");
            }
        }

        void valueChanged(juce::Slider *slider) {
            if (slider) {
                slider->repaint(); // Request a repaint
            }
        }

    private:
        tdTrackController *owner;
    };

    trackDeckMainProcessor &processor;
    juce::OwnedArray<apcToggleParameterButton> squares;
    juce::Image shadowImage;
    std::unique_ptr<CustomSlider> slider1, slider2;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> slider1_attachment, slider2_attachment;
    std::unique_ptr<CustomSliderLookAndFeel> customSliderLookAndFeel;
    const int stepNumber;
};
