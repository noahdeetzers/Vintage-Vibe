/*
  ==============================================================================

    SliderImage.h
    Created: 7 Mar 2023 12:03:30am
    Author:  Noah Deetz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class ValueSliderLNF : public juce::LookAndFeel_V4
{
public:
    ValueSliderLNF()
    {
        setColour (juce::Slider::thumbColourId, juce::Colours::transparentWhite);
        setColour (juce::Slider::rotarySliderFillColourId, juce::Colours::transparentWhite);
        setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colours::transparentWhite);
        setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
    }
};



class ComponentFilmstrip  : public juce::Component
{
public:
    ComponentFilmstrip(juce::Slider *slider, juce::Image* imageToAttachTo, int sliderWidth, int sliderHeight, int frameCount, float scaleFactor);
    
    ComponentFilmstrip(juce::Button *toggleButton, juce::Image* imageToAttachTo, int toggleButtonWidth, int toggleButtonHeight, int frameCount, float scaleFactor);
    
    ComponentFilmstrip(juce::ComboBox *comboBox, juce::Image* imageToAttachTo, int comboBoxWidth, int comboBoxHeight, int frameCount, float scaleFactor);
    
    ~ComponentFilmstrip() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void setNewRange(float rangeMin, float rangeMax);
    

private:
    juce::Slider *mSlider = nullptr;
    juce::Button *mToggleButton = nullptr;
    juce::ComboBox *mComboBox = nullptr;
    
    float sliderMaxVal;
    float sliderMinVal;
    float currentSliderVal;
    
    float mScaleFactor;
    int mFrameWidth, mFrameHeight, mFrameCount;
    
    juce::String stripType;
    
    juce::Image filmStrip;
    bool isVertical = false;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComponentFilmstrip)
};
