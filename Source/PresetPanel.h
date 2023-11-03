/*
  ==============================================================================

    PresetPanel.h
    Created: 5 Feb 2023 6:00:47pm
    Author:  Noah Deetz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
//==============================================================================
/*
*/

class PresetPanel  : public juce::Component, juce::Button::Listener, juce::ComboBox::Listener
{
public:
    PresetPanel(PresetManager& pm);
    ~PresetPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void buttonClicked(juce::Button* button) override;
    void comboBoxChanged(juce::ComboBox* combobox) override;

//    PresetManager& presetManager;
    
    void increment();
    void decrement();
    
private:
    
    void configureButton(juce::Button& button, const juce::String& text);
    
    void loadPresetList();
    
    juce::TextButton saveButton, deleteButton, previousPresetButton, nextPresetButton;
    juce::ComboBox presetList;
    
    PresetManager& presetManager;
    
    std::unique_ptr<juce::FileChooser> fileChooser;
    
    juce::AlertWindow deletionWarning {"const String &title", "const String &message",juce::MessageBoxIconType::WarningIcon,nullptr};

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetPanel)
};
