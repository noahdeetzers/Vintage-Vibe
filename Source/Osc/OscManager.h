/*
  ==============================================================================

    OscManager.h
    Created: 25 May 2023 4:03:37pm
    Author:  Noah Deetz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class OscManager  : public juce::Component
{
public:
    OscManager();
    ~OscManager() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void configureButton(juce::Button &button, const juce::String &text);
    
    int getPortNumber();
    bool getOscStatus();

private:
    
    juce::TextButton oscButton;
    juce::AlertWindow oscPopup {"OSC Remote Settings", "Connect OSC control to IP port: ", juce::MessageBoxIconType::NoIcon, &oscButton};
    
    juce::String ipAddress {juce::IPAddress::getLocalAddress().toString()};
    juce::String oscPortNumber = "57130";
    bool oscStatus = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscManager)
};
