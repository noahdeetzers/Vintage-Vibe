/*
  ==============================================================================

    LevelMeter.cpp
    Created: 23 Apr 2023 8:34:53pm
    Author:  Noah Deetz

  ==============================================================================
*/

#include <JuceHeader.h>
#include "LevelMeter.h"

//==============================================================================
LevelMeter::LevelMeter()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

LevelMeter::~LevelMeter()
{
}

void LevelMeter::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    g.setColour(juce::Colours::white.withBrightness(0.25f));
    g.fillRoundedRectangle(bounds, 5.0f);

    g.setColour(juce::Colours::white);
               

    //map level from -60.0 / 6.0 to zero and width --- you need to translate the range

    const auto scaledY = juce::jmap(level, -60.0f, +6.0f, 0.0f,static_cast<float>(getHeight()));
    
    g.fillRoundedRectangle(bounds.removeFromBottom(scaledY), 5.0f);
    
    
    
//    // draw some placeholder text
}

void LevelMeter::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}


void LevelMeter::setLevel(float currentLevel)
{
    level = currentLevel;
}

