/*
  ==============================================================================

    SliderImage.cpp
    Created: 7 Mar 2023 12:03:30am
    Author:  Noah Deetz

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ComponentFilmstrip.h"

//==============================================================================
ComponentFilmstrip::ComponentFilmstrip(juce::Slider *slider, juce::Image* imageToAttachTo, int sliderWidth, int sliderHeight, int frameCount, float scaleFactor)
{
////    Handles what slider it is
//    if (sliderName == "Pedal Input Gain")
//    {
//        filmStrip = juce::ImageCache::getFromMemory(BinaryData::PedalInputGain_png, BinaryData::PedalInputGain_pngSize);
//    }
//    if (sliderName == "Pedal Output Gain")
//    {
//        filmStrip = juce::ImageCache::getFromMemory(BinaryData::PedalOutputGain_png, BinaryData::PedalOutputGain_pngSize);
//    }
    
    
    filmStrip = *imageToAttachTo;
        

    mSlider = slider;
    mFrameWidth = sliderWidth;
    mFrameHeight = sliderHeight;
    mFrameCount = frameCount;
    mScaleFactor = scaleFactor;
    
    sliderMaxVal = mSlider->getMaximum();
    sliderMinVal = mSlider->getMinimum();

    DBG(sliderMaxVal);
    DBG(sliderMinVal);
    
    stripType = "slider";
}


ComponentFilmstrip::ComponentFilmstrip(juce::Button *toggleButton, juce::Image* imageToAttachTo, int toggleButtonWidth, int toggleButtonHeight, int frameCount, float scaleFactor)
{
//    if (toggleButtonName == "Bypass")
//    {
//        filmStrip = juce::ImageCache::getFromMemory(BinaryData::bypassButton_png, BinaryData::bypassButton_pngSize);
//    }
    
    
    filmStrip = *imageToAttachTo;
    
    
    mToggleButton = toggleButton;
    mFrameWidth = toggleButtonWidth;
    mFrameHeight = toggleButtonHeight;
    mFrameCount = frameCount;
    mScaleFactor = scaleFactor;
    
    stripType = "toggleButton";
    
    
//    DBG("button loaded");
}

ComponentFilmstrip::ComponentFilmstrip(juce::ComboBox *comboBox, juce::Image* imageToAttachTo, int comboBoxWidth, int comboBoxHeight, int frameCount, float scaleFactor)
{
    
    filmStrip = *imageToAttachTo;
    
    mComboBox = comboBox;
    mFrameWidth = comboBoxWidth;
    mFrameHeight = comboBoxHeight;
    mFrameCount = frameCount;
    mScaleFactor = scaleFactor;
    
    stripType = "comboBox";

}



ComponentFilmstrip::~ComponentFilmstrip()
{
}

void ComponentFilmstrip::paint (juce::Graphics& g)
{
    auto xPos = 0;
    auto yPos = 0;
    
    if (stripType == "slider")
    {
        currentSliderVal = mSlider->getValue();

        
//        const double fractRotation =  (currentSliderVal - sliderMinVal) / (sliderMaxVal - sliderMinVal);
        const float fractRotation = (float) mSlider->valueToProportionOfLength(currentSliderVal);
        
//        DBG(fractRotation);
    
        //converts fractRotation from 0-1 to 0-the total amount of frames in the filmstrip
//        int pos = (int)ceil(fractRotation * ((double)mFrameCount - 1.0));
        int pos = fractRotation * (mFrameCount - 1);

        g.drawImage(filmStrip, xPos, yPos, mFrameWidth * mScaleFactor, mFrameHeight * mScaleFactor, 0, (int)pos * mFrameHeight, mFrameWidth, mFrameHeight, false); // Draws  strip while shifting height 
        
    }
    else if (stripType == "toggleButton")
    {
        
//        Toggle buttons have three states: off, over and on.
//        for the pedals, the "over state is a snapshot of it 2 frames over in blender
        
        g.drawImage(filmStrip, xPos, yPos, mFrameWidth * mScaleFactor, mFrameHeight * mScaleFactor, 0,0 * mFrameHeight, mFrameWidth, mFrameHeight, false); // Draws the strip while shifting the height depending on the film strip
        
        if (mToggleButton->isOver())
        {
            g.drawImage(filmStrip, xPos, yPos, mFrameWidth * mScaleFactor, mFrameHeight * mScaleFactor, 0,1 * mFrameHeight, mFrameWidth, mFrameHeight, false); // Draws the strip while shifting the height depending on the film strip
        }
        if (mToggleButton->getToggleState())
        {
            g.drawImage(filmStrip, xPos, yPos, mFrameWidth * mScaleFactor, mFrameHeight * mScaleFactor, 0,2 * mFrameHeight, mFrameWidth, mFrameHeight, false); // Draws the strip while shifting the height depending on the film strip
        }

    } else if (stripType == "comboBox")
    {
        auto currIndex = mComboBox->getSelectedItemIndex();
        
        g.drawImage(filmStrip, xPos, yPos, mFrameWidth * mScaleFactor, mFrameHeight * mScaleFactor, 0,currIndex * mFrameHeight, mFrameWidth, mFrameHeight, false); // Draws the strip while shifting the height depending on the film strip
    }
    
    if (this->isMouseOver() == false)
    {
        repaint();
//        DBG("repainting");
    } else {
//        DBG("not repainting");
    }
}

void ComponentFilmstrip::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void ComponentFilmstrip::setNewRange(float rangeMin, float rangeMax)
{
    sliderMaxVal = rangeMax;
    sliderMinVal = rangeMin;
}

