/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ComponentFilmstrip.h"
#include "LevelMeter.h"
#include "PresetPanel.h"
#include "../Source/Osc/OscManager.h"

//==============================================================================
/**
*/
class GuitarOscAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        public juce::OSCReceiver,
                                    public juce::OSCReceiver::ListenerWithOSCAddress<juce::OSCReceiver::MessageLoopCallback>
{
public:
    GuitarOscAudioProcessorEditor (GuitarOscAudioProcessor&);
    ~GuitarOscAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    
    
//    PV function for osc
    void oscMessageReceived( const juce::OSCMessage &message) override;
    void showPage(int pageNum);

//    Sets the properties of the components for cleaner code
    void setLabelProperties(const juce::String& name, juce::Label& label);
    
//    Helper function to set visibility of components
    void setVisibility(int pageId);
    
    void setupSlider(juce::Slider& slider, juce::Component& image);
    void setupButton(juce::Button& button, juce::Component& image);

//    Slider listener function
//    void setDelaySliderVal();
    
    void connectOSC();
    int currentOSCNumber;

private:
    
//    PAGE SELECTION CONTROLS
    juce::ImageButton mPageSelectors [2];
    int currentPage = 1;
    
    juce::Image mAmpPageSelectorImage = juce::ImageCache::getFromMemory(BinaryData::Amp_png, BinaryData::Amp_pngSize);
    juce::Image mFxPageSelectorImage = juce::ImageCache::getFromMemory(BinaryData::Fx_png, BinaryData::Fx_pngSize);
    
//    INFO Button
    juce::ImageButton mInfoButton;
    juce::Image mInfoButtonImage = juce::ImageCache::getFromMemory(BinaryData::Info_png, BinaryData::Info_pngSize);
    
//    AMP HEAD CONTROLS
    juce::Slider mInputGainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mInputGainSliderAttachement;
    juce::LinearSmoothedValue<float> accelXSmoothed {0.0f};
    
    juce::Slider mBassGainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mBassGainSliderAttachement;
    
    juce::Slider mMidGainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mMidGainSliderAttachement;
    
    juce::Slider mTrebleGainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mTrebleGainSliderAttachement;
    
    juce::ToggleButton mAmpBypassButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mAmpBypassButtonAttachment;
    
    juce::ToggleButton mPlus15DbButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mPlus15DbButtonAttachement;

    juce::Slider mGlobalOutputGainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mGlobalOutputGainSliderAttachment;
    
//    OVERDRIVE CONTROLS
    juce::Slider mOverDriveAmountSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mOverDriveAmountSliderAttachment;
    juce::Label mOverDriveAmountSldierLabel;
    
    juce::Slider mOverDriveBiasSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mOverDriveBiasSliderAttachment;
    juce::Label mOverDriveBiasSldierLabel;
    
    juce::Slider mOverDriveToneSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mOverDriveToneSliderAttachment;
    juce::Label mOverDriveToneSldierLabel;
    
    juce::Slider mOverDriveLevelSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mOverDriveLevelSliderAttachment;
    juce::Label mOverDriveLevelSldierLabel;
    
    juce::ToggleButton mOverDriveBypassButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mOverDriveBypassButtonAttachment;
    juce::Label mOverDriveBypassButtonLabel;
    
//    REVERB CONTROLS
    juce::ComboBox mReverbSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> mReverbSelectorAttachement;

    juce::Slider mReverbAmountSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mReverbAmountSliderAttachment;
    juce::Label mReverbAmountLabel;
    juce::Slider mReverbLPCutoffSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mReverbLPCutoffSliderAttachment;
    juce::Label mReverbLPCutoffLabel;
    juce::Slider mReverbHPCutoffSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mReverbHPCutoffSliderAttachment;
    juce::Label mReverbHPCutoffLabel;
    
    juce::TextButton mReverbIncrementButton;
    juce::TextButton mReverbDecrementButton;
    
//    HAAS DELAY CONTROLS
    juce::Slider mHaasDelaySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mHaasDelaySliderAttachment;

    juce::ToggleButton mHaasBypassButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mHaasBypassButtonAttachment;
    
    juce::ToggleButton mHaasSideButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mHaasSideButtonAttachment;

    juce::ToggleButton mReverbBypassButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mReverbBypassButtonAttachment;

//    DELAY CONTROLS
    juce::Slider mDelayAmountSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mDelayAmountSliderAttachment;
    
    juce::Slider mDelayTimeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mDelayTimeSliderAttachment;
    
    juce::Slider mDelayFeedbackSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mDelayFeedbackAttachment;

    juce::Slider mDelayLPFreqSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mDelayLPFreqSliderAttachment;
    
    juce::Slider mDelayHPFreqSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mDelayHPFreqSliderAttachment;
    
    juce::ToggleButton mDelayBypassButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mDelayBypassButtonAttachement;
    
    juce::Slider mDelayWidthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mDelayWidthSliderAttachment;
    
    juce::ToggleButton mDelaySnapToggle;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mDelaySnapToggleAttachment;
    
    juce::Slider mDelayTimeSnappedSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mDelayTimeSnappedSliderAttachment;

//    GRAPHICS
//    juce::Image page1Image = juce::ImageFileFormat::loadFrom(BinaryData::_317Render_jpg, BinaryData::_317Render_jpgSize);
    juce::Image page1Image = juce::ImageFileFormat::loadFrom(BinaryData::AmpBackground_png, BinaryData::AmpBackground_pngSize);

//    juce::Image page2Image = juce::ImageFileFormat::loadFrom(BinaryData::backgroundwithoutshadow_png, BinaryData::backgroundwithoutshadow_pngSize);
    juce::Image page2Image = juce::ImageFileFormat::loadFrom(BinaryData::pedals2_png, BinaryData::pedals2_pngSize);
    ValueSliderLNF valueSliderLNF;
    
    float scaleFactor = 0.265;
//    used to be 0.26
    int numKnobFrames = 50;
//    PEDALS
    int pedalKnobWidth = 221, pedalKnobHeight = 11950 / 50;
    juce::Image mPedalKnobFs = juce::ImageCache::getFromMemory(BinaryData::overDriveInputGain_png, BinaryData::overDriveInputGain_pngSize);
    int pedalBypassButtonWidth = 487, pedalBypassButtonHeight = 876 / 3;
    juce::Image mPedalBypassButtonFs = juce::ImageCache::getFromMemory(BinaryData::bypassButton_png, BinaryData::bypassButton_pngSize);
    
    int delaySnapToggleWidth = 59, delaySnapToggleHeight = 576 / 3;
    juce::Image mDelaySnapToggleFs = juce::ImageCache::getFromMemory(BinaryData::delaySyncSwitch1_png, BinaryData::delaySyncSwitch1_pngSize);
    
    
    //    overdrive
    ComponentFilmstrip mOverDriveAmountImage {&mOverDriveAmountSlider, &mPedalKnobFs, pedalKnobWidth, pedalKnobHeight, numKnobFrames, scaleFactor};
    ComponentFilmstrip mOverDriveBiasImage {&mOverDriveBiasSlider, &mPedalKnobFs, pedalKnobWidth, pedalKnobHeight, numKnobFrames, scaleFactor};
    ComponentFilmstrip mOverDriveLevelImage {&mOverDriveLevelSlider, &mPedalKnobFs, pedalKnobWidth, pedalKnobHeight, numKnobFrames, scaleFactor};
    ComponentFilmstrip mOverDriveToneImage {&mOverDriveToneSlider, &mPedalKnobFs, pedalKnobWidth, pedalKnobHeight, numKnobFrames, scaleFactor};
    ComponentFilmstrip mOverDriveBypassButtonImage {&mOverDriveBypassButton, &mPedalBypassButtonFs, pedalBypassButtonWidth, pedalBypassButtonHeight, 3, scaleFactor};

    //    reverb
    ComponentFilmstrip mReverbAmountImage {&mReverbAmountSlider, &mPedalKnobFs, pedalKnobWidth, pedalKnobHeight, numKnobFrames, scaleFactor};
    ComponentFilmstrip mReverbHPCutoffImage {&mReverbHPCutoffSlider, &mPedalKnobFs, pedalKnobWidth, pedalKnobHeight, numKnobFrames, scaleFactor};
    ComponentFilmstrip mReverbLPCutoffImage {&mReverbLPCutoffSlider, &mPedalKnobFs, pedalKnobWidth, pedalKnobHeight, numKnobFrames, scaleFactor};
    ComponentFilmstrip mReverbBypassButtonImage {&mReverbBypassButton, &mPedalBypassButtonFs, pedalBypassButtonWidth, pedalBypassButtonHeight, 3, scaleFactor};
    
    int reverbSelectorWidth = 248, reverbSelectorHeight = 336 / 6;
    juce::Image mReverbSelectorFs = juce::ImageCache::getFromMemory(BinaryData::reverbSelectorText_png, BinaryData::reverbBypass_pngSize);
    ComponentFilmstrip mReverbSelectorImage {&mReverbSelector, &mReverbSelectorFs, reverbSelectorWidth, reverbSelectorHeight, 2, scaleFactor};
    
    int reverbIncrementWidth = 89, reverbIncrementHeight = 279 / 3;
    juce::Image mReverbIncrementFs = juce::ImageCache::getFromMemory(BinaryData::reverbSelectorRight_png, BinaryData::reverbSelectorRight_pngSize);
    ComponentFilmstrip mReverbIncrementButtonImage {&mReverbIncrementButton, &mReverbIncrementFs, reverbIncrementWidth, reverbIncrementHeight, 3, scaleFactor};

    int reverbDecrementWidth = 93, reverbDecrementHeight = 279 / 3;
    juce::Image mReverbDecrementFs = juce::ImageCache::getFromMemory(BinaryData::reverbSelectorLeft_png, BinaryData::reverbSelectorLeft_pngSize);
    ComponentFilmstrip mReverbDecrementButtonImage {&mReverbDecrementButton, &mReverbDecrementFs, reverbDecrementWidth, reverbDecrementHeight, 3, scaleFactor};
    
    //    delay
    ComponentFilmstrip mDelayAmountImage {&mDelayAmountSlider, &mPedalKnobFs, pedalKnobWidth, pedalKnobHeight, numKnobFrames, scaleFactor};
    ComponentFilmstrip mDelayTimeImage {&mDelayTimeSlider, &mPedalKnobFs, pedalKnobWidth, pedalKnobHeight, numKnobFrames, scaleFactor};
    ComponentFilmstrip mDelayTimeSnappedImage {&mDelayTimeSnappedSlider, &mPedalKnobFs, pedalKnobWidth, pedalKnobHeight, numKnobFrames, scaleFactor};
    ComponentFilmstrip mDelayFeedbackImage {&mDelayFeedbackSlider, &mPedalKnobFs, pedalKnobWidth, pedalKnobHeight, numKnobFrames, scaleFactor};
    ComponentFilmstrip mDelayHPFreqImage {&mDelayHPFreqSlider, &mPedalKnobFs, pedalKnobWidth, pedalKnobHeight, numKnobFrames, scaleFactor};
    ComponentFilmstrip mDelayLPFreqImage {&mDelayLPFreqSlider, &mPedalKnobFs, pedalKnobWidth, pedalKnobHeight, numKnobFrames, scaleFactor};
    ComponentFilmstrip mDelayWidthImage {&mDelayWidthSlider, &mPedalKnobFs, pedalKnobWidth, pedalKnobHeight, numKnobFrames, scaleFactor};
    ComponentFilmstrip mDelayBypassButtonImage {&mDelayBypassButton, &mPedalBypassButtonFs, pedalBypassButtonWidth, pedalBypassButtonHeight, 3, scaleFactor};
    
    ComponentFilmstrip mDelaySnapToggleImage {&mDelaySnapToggle, &mDelaySnapToggleFs, delaySnapToggleWidth, delaySnapToggleHeight, 3, scaleFactor};
    
//    AMP GRAPHICS
    int ampBypassButtonWidth = 134, ampBypassButtonHeight = 255 / 3;
    juce::Image mAmpBypassButtonFs = juce::ImageCache::getFromMemory(BinaryData::Amp_Bypass_Button_png, BinaryData::Amp_Bypass_Button_pngSize);
    ComponentFilmstrip mAmpBypassButtonImage {&mAmpBypassButton, &mAmpBypassButtonFs, ampBypassButtonWidth, ampBypassButtonHeight, 3, scaleFactor};
    
    int plus15DbButtonWidth = 134, plus15DbButtonHeight = 255 / 3;
    juce::Image mPlus15DbButtonFs = juce::ImageCache::getFromMemory(BinaryData::_15DbButton_png, BinaryData::_15DbButton_pngSize);
    ComponentFilmstrip mPlus15DbButtonImage {&mPlus15DbButton, &mPlus15DbButtonFs, plus15DbButtonWidth, plus15DbButtonHeight, 3, scaleFactor};
    
//    int ampInputGainWidth = 158, ampInputGainHeight = 7200 / 50;
    int ampInputGainWidth = 240, ampInputGainHeight = 8650 / 50;
//    juce::Image mAmpInputGainFs = juce::ImageCache::getFromMemory(BinaryData::inputGain_png, BinaryData::inputGain_pngSize);
    juce::Image mAmpInputGainFs = juce::ImageCache::getFromMemory(BinaryData::new_inputGain_png, BinaryData::new_inputGain_pngSize);
    ComponentFilmstrip mAmpInputGainImage {&mInputGainSlider, &mAmpInputGainFs, ampInputGainWidth, ampInputGainHeight, numKnobFrames, scaleFactor};
    
//    int bassGainWidth = 158, bassGainHeight = 7600 / 50;
    int bassGainWidth = 212, bassGainHeight = 7650 / 50;
//    juce::Image mBassGainFs = juce::ImageCache::getFromMemory(BinaryData::bassGain_png, BinaryData::bassGain_pngSize);
    juce::Image mBassGainFs = juce::ImageCache::getFromMemory(BinaryData::new_bass_gain_png, BinaryData::new_bass_gain_pngSize);
    ComponentFilmstrip mBassGainImage {&mBassGainSlider, &mBassGainFs, bassGainWidth, bassGainHeight, numKnobFrames, scaleFactor};
    
//    int midGainWidth = 180, midGainHeight = 8750 / 50;
    int midGainWidth = 196, midGainHeight = 8050 / 50;
    juce::Image mMidGainFs = juce::ImageCache::getFromMemory(BinaryData::new_mid_gain_png, BinaryData::new_mid_gain_pngSize);
//    juce::Image mMidGainFs = juce::ImageCache::getFromMemory(BinaryData::midGain_png, BinaryData::midGain_pngSize);
    ComponentFilmstrip mMidGainImage {&mMidGainSlider, &mMidGainFs, midGainWidth, midGainHeight, numKnobFrames, scaleFactor};
    
//    int trebleGainWidth = 180, trebleGainHeight = 8300 / 50;
    int trebleGainWidth = 240, trebleGainHeight = 7650 / 50;
    juce::Image mTrebleGainFs = juce::ImageCache::getFromMemory(BinaryData::new_treble_gain_png, BinaryData::new_treble_gain_pngSize);
//    juce::Image mTrebleGainFs = juce::ImageCache::getFromMemory(BinaryData::trebleGain_png, BinaryData::trebleGain_pngSize);
    ComponentFilmstrip mTrebleGainImage {&mTrebleGainSlider, &mTrebleGainFs, trebleGainWidth, trebleGainHeight, numKnobFrames, scaleFactor};
    
//    int outputGainWidth = 180, OutputGainHeight = 8300 / 50;
    int outputGainWidth = 216, OutputGainHeight = 7850 / 50;
    juce::Image mOutputGainFs = juce::ImageCache::getFromMemory(BinaryData::new_output_gain_png, BinaryData::new_output_gain_pngSize);
//    juce::Image mOutputGainFs = juce::ImageCache::getFromMemory(BinaryData::outputVolumeGain_png, BinaryData::outputVolumeGain_pngSize);
    ComponentFilmstrip mOutputGainImage {&mGlobalOutputGainSlider, &mOutputGainFs, outputGainWidth, OutputGainHeight, numKnobFrames, scaleFactor};
    
    int doublerWidthWidth = 188, doublerWidthHeight = 11050 / 50;
    juce::Image mDoublerWidthFs = juce::ImageCache::getFromMemory(BinaryData::Doubler_Amount_png, BinaryData::Doubler_Amount_pngSize);
    ComponentFilmstrip mDoublerWidthImage {&mHaasDelaySlider, &mDoublerWidthFs, doublerWidthWidth, doublerWidthHeight, numKnobFrames, scaleFactor};
    
    int doublerBypassButtonWidth = 104, doublerBypassButtonHeight = 255 / 3;
    juce::Image mDoublerBypassButtonFs = juce::ImageCache::getFromMemory(BinaryData::doubler_Bypass_png, BinaryData::doubler_Bypass_pngSize);
    ComponentFilmstrip mDoublerBypassButtonImage {&mHaasBypassButton, &mDoublerBypassButtonFs, doublerBypassButtonWidth, doublerBypassButtonHeight, 3, scaleFactor};
    
    int doublerSideButtonWidth = 103, doublerSideButtonHeight = 252 / 3;
    juce::Image mDoublerSideButtonFs = juce::ImageCache::getFromMemory(BinaryData::doubler_Side_png, BinaryData::doubler_Side_pngSize);
    ComponentFilmstrip mDoublerSideBypassButtonImage {&mHaasSideButton, &mDoublerSideButtonFs, doublerSideButtonWidth, doublerSideButtonHeight, 3, scaleFactor};
    
//    Mapping Section    
    juce::StringArray oscPaths {"/AmpToggle", "/AMP/InputGain", "/AMP/OutputGain",  "/AMP/BassGain", "/AMP/MidGain",
        "/AMP/TrebleGain", "/AMP/DoublerWidth",
        
        "/FX/OverdriveBypass", "/FX/InputGain", "/FX/OutputGain", "/FX/Bias", "/FX/Tone",
        
        "/FX/ReverbBypass", "/FX/ReverbMix", "/FX/ReverbHPF", "/FX/ReverbLPF", "/FX/ReverbSelector",
        
        "/FX/DelayBypass", "/FX/DelayMix", "/FX/DelayHPF", "/FX/DelayLPF", "/FX/DelayTime", "/FX/DelaySpread", "/FX/DelayFeedback",
        
        "/AMP/CabBypass", "/AMP/Plus15Db", "/AMP/DoublerBypass", "/AMP/DoublerSide", "/AMP/PresetIncrementButton", "/AMP/PresetDecrementButton",
        
        "/FX/TempoSyncToggle", "/FX/TempoSyncValue"
    };
    
    // level meter
    LevelMeter preAmpLevelMeters [2];
    LevelMeter postAmpLevelMeters [2];
    
//    Preset Panel
    PresetPanel presetPanel;
    
//    Osc Manager
    OscManager oscManager;
    
    GuitarOscAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarOscAudioProcessorEditor)
};
