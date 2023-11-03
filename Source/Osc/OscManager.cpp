/*
  ==============================================================================

    OscManager.cpp
    Created: 25 May 2023 4:03:37pm
    Author:  Noah Deetz

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OscManager.h"

//==============================================================================
OscManager::OscManager()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    oscPopup.addTextEditor("PORTNUMBER", oscPortNumber);
    
//    const juce::MessageManagerLock mmLock;
    
    configureButton(oscButton, "OSC");
    oscButton.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::red);
    oscPopup.setEnabled(false); // had to set it to initially not be enabled becuase it grabs keyboard focus - make sure in future projects to always set components disabled until they need to be because this bug was a PAIN IN THE ASS
    oscButton.onClick = [this]
    {
        if (oscPopup.isVisible())
        {
            oscPopup.setVisible(false);
            oscPopup.setEnabled(false);
        } else
        {
            oscPopup.setVisible(true);
            oscPopup.setEnabled(true);
            oscPopup.enterModalState (false, juce::ModalCallbackFunction::create ([this](int result) {
                if (result == 0) {
                    oscStatus = false;
                    oscButton.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::red);
                    oscPopup.setVisible(false);
                }// result == 0 means you click Cancel
                if (result == 1){
                    oscStatus = true;
                    oscButton.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::green);
                    oscPopup.setVisible(false);
                }
            }));
        }
    };
    
    oscPopup.setColour(juce::AlertWindow::backgroundColourId, juce::Colours::white.withBrightness(0.25f));
    oscPopup.addButton("Enable", 1);
    oscPopup.addButton ("Disable", 0);
    addAndMakeVisible(oscPopup);
    oscPopup.setVisible(false);
}

OscManager::~OscManager()
{
}

void OscManager::paint (juce::Graphics& g)
{
    oscPortNumber = oscPopup.getTextEditorContents("PORTNUMBER");
    oscPopup.setMessage("IP address: " + ipAddress + "\n" + "port number: " + oscPortNumber);
}

void OscManager::resized()
{

    oscButton.setBounds(6,530,60,25);
//    oscButton.setBounds(getLocalBounds());
    
    
//    oscPopup.setBounds(getParentWidth() / 2 - 150, getParentHeight() / 2 - 150, 300, 300);
//    TODO: Get the right bounds set for the OSC popup box
    
}
void OscManager::configureButton(juce::Button &button, const juce::String &text)
{
    button.setButtonText(text);
    button.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    button.setColour(juce::TextButton::buttonColourId, juce::Colours::white.withBrightness(0.25f));
    addAndMakeVisible(button);
}
int OscManager::getPortNumber()
{
    // Check if the oscPortNumber is not empty and contains only digits
    if (oscPortNumber.isNotEmpty() && oscPortNumber.containsOnly("0123456789"))
    {
        // Convert the string to an integer and return it
        DBG(oscPortNumber);
        return oscPortNumber.getIntValue();
    }

    // Return a default value or handle the error case
    return -1; // Or any other default value/error handling logic
}
bool OscManager::getOscStatus(){
    return oscStatus;
}
