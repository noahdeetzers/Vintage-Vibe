/*
  ==============================================================================

    LevelMeter.h
    Created: 23 Apr 2023 8:34:53pm
    Author:  Noah Deetz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class LevelMeter  : public juce::Component
{
public:
    LevelMeter();
    ~LevelMeter() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    
    void setLevel(const float currentLevel);

private:
    float level = -60.0f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelMeter)
};

