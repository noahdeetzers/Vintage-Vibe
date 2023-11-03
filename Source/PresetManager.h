/*
  ==============================================================================

    PresetManager.h
    Created: 5 Feb 2023 6:29:21pm
    Author:  Noah Deetz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
 This is a basic presetManager and PresetPanel for an audio plugin. User presets are stored in a folder,
 and factory presets are stored in BinaryData. Here is what other code goes in the different JUCE file structure:
 
 PluginEditor.h ---------------------------------------------------------------------------------------------------------------------------------------
             PresetPanel presetPanel;
 
 PluginEditor.cpp --------------------------------------------------------------------------------------------------------------------------------------
             JucePresetManagerAudioProcessorEditor::JucePresetManagerAudioProcessorEditor (JucePresetManagerAudioProcessor& p) :
                 AudioProcessorEditor(&p),
                 audioProcessor(p),
                 genericAudioProcessorEditor(p),
                 presetPanel(p.getPresetManager()) <--------------- YOU NEED TO INITIALIZE IT IN THE INIT LIST
             {
                 addAndMakeVisible(presetPanel);
 
 PluginProcessor.h -------------------------------------------------------------------------------------------------------------------------------------
            public:
             PresetManager& getPresetManager() { return *presetManager;};

            private:
             juce::AudioProcessorValueTreeState valueTreeState;
 
            //    Create instance of the presetManager obj
             std::unique_ptr<PresetManager> presetManager;

 PluginProcessor.cpp -----------------------------------------------------------------------------------------------------------------------------------
             JucePresetManagerAudioProcessor::JucePresetManagerAudioProcessor() :
                 AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
             {
                 valueTreeState.state.setProperty(PresetManager::presetNameProperty, "", nullptr); <----- SETS THE PRESETNAMEPROPERTY
                 valueTreeState.state.setProperty("version", ProjectInfo::versionString, nullptr); <----- SETS THE VERSION FOR COMPATIBILITY
                 
                 presetManager = std::make_unique<PresetManager>(valueTreeState); <----- INITIALIZE UNIQUE POINTERS HOLDING THE PRESET MANAGER
                 
             }
*/
class PresetManager : juce::ValueTree::Listener
{
public:
    PresetManager(juce::AudioProcessorValueTreeState&);
    
    void savePreset(const juce::String& presetName);
    void deletePreset(const juce::String& presetName);
    void loadPreset(const juce::String& presetName);
    int loadNextPreset();
    int loadPreviousPreset();
    juce::StringArray getAllPresets() const;
    juce::String getCurrentPreset() const;
    

    
    static const juce::File defaultDirectory;
    static const juce::String extension;
    static const juce::String presetNameProperty;
    
    
    void setComboId(int idx);
    int getComboId();
    int comboId;
    
private:
    void valueTreeRedirected(juce::ValueTree& changedTree) override;
    
    
    
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::Value currentPreset;
    
    
//    const char* factoryPresets[4] = {
//        BinaryData::Country_Twang_preset,
//        BinaryData::Dry_n_Clean_preset,
//        BinaryData::Massive_Roomy_Rhythm_preset,
//        BinaryData::RnB_Sparkle_preset
//        // Add more binary data objects here if needed
//    };
    
    const char* factoryPresets[18] = {
        BinaryData::Dry_n_Clean_preset,
        BinaryData::Backseat_Indie_Rhythms_preset,
        BinaryData::Country_Twang_preset,
        BinaryData::Clean_Double_Indie_Goodness_preset,
        BinaryData::_50_s_Midrange_and_Reverb_preset,
        BinaryData::Indie_Plate_Verb_preset,
        BinaryData::Jazz_or_Gospel_Tone_preset,
        BinaryData::Massive_Roomy_Rhythm_preset,
        BinaryData::Midwest_Emo_Breakup_Rhythms__preset,
        BinaryData::Midwest_Emo_Clean_Tele_Tone_preset,
        BinaryData::PalmMuted_Indie_Power_Chords_preset,
        BinaryData::RnB_Sparkle_preset,
        BinaryData::Shoegaze_Clean_Rhythms__preset,
        BinaryData::Shoegaze_Leads_preset,
        BinaryData::Stroke_Punk_preset,
        BinaryData::Tweed_Classic_Rock_Tone_preset,
        BinaryData::Under_The_Sea_preset,
        BinaryData::Wet_Clean_8th_Delay_preset
        // Add more binary data objects here if needed
    };

    const size_t numElements = sizeof(factoryPresets) / sizeof(factoryPresets[0]);
    
    
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetManager)
};
