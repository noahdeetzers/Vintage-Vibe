/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GuitarOscAudioProcessorEditor::GuitarOscAudioProcessorEditor (GuitarOscAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), presetPanel(p.getPresetManager())
{
    mPageSelectors[0].setAlwaysOnTop(true);
    mPageSelectors[1].setAlwaysOnTop(true);
    addAndMakeVisible(mPageSelectors[0]);
    addAndMakeVisible(mPageSelectors[1]);
    
    mPageSelectors[0].onClick = [this] {
        currentPage = 1;
        mPageSelectors[0].setState(juce::Button::buttonDown);
    };
    mPageSelectors[1].onClick = [this] {
        currentPage = 2;
        mPageSelectors[1].setState(juce::Button::buttonDown);
    };
    
    mPageSelectors[0].setImages(false, true, true, mAmpPageSelectorImage, 1.0f, juce::Colours::transparentWhite, mAmpPageSelectorImage, 0.8f, juce::Colours::grey, mAmpPageSelectorImage, 0.5f, juce::Colours::grey);
    mPageSelectors[1].setImages(false, true, true, mFxPageSelectorImage, 1.0f, juce::Colours::transparentWhite, mFxPageSelectorImage, 0.8f, juce::Colours::grey, mFxPageSelectorImage, 0.5f, juce::Colours::grey);
    
//    Info Button
    mInfoButton.setAlwaysOnTop(true);
    addAndMakeVisible(mInfoButton);
    mInfoButton.setImages(false, true, true, mInfoButtonImage, 1.0f, juce::Colours::transparentWhite, mInfoButtonImage, 0.8f, juce::Colours::grey, mInfoButtonImage, 0.5f, juce::Colours::grey);
    
    mInfoButton.onClick = [this] {
        juce::URL url {"https://fineclassicsplugins.com/pages/vintage-vibe-plugin-user-manual"};
        url.launchInDefaultBrowser();
    };
    
    
//    AMP HEAD ------------------------------------------------------------------------------------------------------------------------------------
    mAmpInputGainImage.setNewRange(audioProcessor.apvts.getParameterRange("INPUTGAIN").start, audioProcessor.apvts.getParameterRange("INPUTGAIN").end);
    mInputGainSliderAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "INPUTGAIN", mInputGainSlider);
    setupSlider(mInputGainSlider, mAmpInputGainImage);
    
//    POST EQ CONTROLS
    mBassGainImage.setNewRange(audioProcessor.apvts.getParameterRange("BASSGAIN").start, audioProcessor.apvts.getParameterRange("BASSGAIN").end);
    mBassGainSliderAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "BASSGAIN", mBassGainSlider);
    setupSlider(mBassGainSlider, mBassGainImage);
    
    mMidGainImage.setNewRange(audioProcessor.apvts.getParameterRange("MIDGAIN").start, audioProcessor.apvts.getParameterRange("MIDGAIN").end);
    mMidGainSliderAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "MIDGAIN", mMidGainSlider);
    setupSlider(mMidGainSlider, mMidGainImage);

    mTrebleGainImage.setNewRange(audioProcessor.apvts.getParameterRange("TREBLEGAIN").start, audioProcessor.apvts.getParameterRange("TREBLEGAIN").end);
    mTrebleGainSliderAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "TREBLEGAIN", mTrebleGainSlider);
    setupSlider(mTrebleGainSlider, mTrebleGainImage);
    
    mOutputGainImage.setNewRange(audioProcessor.apvts.getParameterRange("OUTPUTGAIN").start, audioProcessor.apvts.getParameterRange("OUTPUTGAIN").end);
    mGlobalOutputGainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "OUTPUTGAIN", mGlobalOutputGainSlider);
    setupSlider(mGlobalOutputGainSlider, mOutputGainImage);
    
//    BYPASS BUTTON
    mAmpBypassButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "CABYPASS", mAmpBypassButton);
    setupButton(mAmpBypassButton, mAmpBypassButtonImage);
    
    mPlus15DbButtonAttachement = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "PLUS15DB", mPlus15DbButton);
    setupButton(mPlus15DbButton, mPlus15DbButtonImage);

    mDoublerWidthImage.setNewRange(audioProcessor.apvts.getParameterRange("HAASDELAYAMOUNT").start, audioProcessor.apvts.getParameterRange("HAASDELAYAMOUNT").end);
    mHaasDelaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "HAASDELAYAMOUNT", mHaasDelaySlider);
    setupSlider(mHaasDelaySlider, mDoublerWidthImage);
    
    //    BYPASS BUTTON
    mHaasBypassButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "HAASBYPASS", mHaasBypassButton);
    setupButton(mHaasBypassButton, mDoublerBypassButtonImage);
    
    //    DOUBLER SIDE BUTTON
    mHaasSideButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "HAASSIDE", mHaasSideButton);
    setupButton(mHaasSideButton, mDoublerSideBypassButtonImage);
    
//    FX -----------------------------------------------------------------------------------------------------------------------------------------
//    OVERDRIVE
    mOverDriveAmountImage.setNewRange(audioProcessor.apvts.getParameterRange("OVERDRIVEAMOUNT").start, audioProcessor.apvts.getParameterRange("OVERDRIVEAMOUNT").end);
    mOverDriveAmountSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "OVERDRIVEAMOUNT", mOverDriveAmountSlider);
    setupSlider(mOverDriveAmountSlider, mOverDriveAmountImage);
    
    mOverDriveBiasImage.setNewRange(audioProcessor.apvts.getParameterRange("OVERDRIVEBIAS").start, audioProcessor.apvts.getParameterRange("OVERDRIVEBIAS").end);
    mOverDriveBiasSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "OVERDRIVEBIAS", mOverDriveBiasSlider);
    setupSlider(mOverDriveBiasSlider, mOverDriveBiasImage);
    
    mOverDriveToneImage.setNewRange(audioProcessor.apvts.getParameterRange("OVERDRIVETONE").start, audioProcessor.apvts.getParameterRange("OVERDRIVETONE").end);
    mOverDriveToneSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "OVERDRIVETONE", mOverDriveToneSlider);
    setupSlider(mOverDriveToneSlider, mOverDriveToneImage);
    
    mOverDriveLevelImage.setNewRange(audioProcessor.apvts.getParameterRange("OVERDRIVELEVEL").start,audioProcessor.apvts.getParameterRange("OVERDRIVELEVEL").end);
    mOverDriveLevelSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "OVERDRIVELEVEL", mOverDriveLevelSlider);
    setupSlider(mOverDriveLevelSlider, mOverDriveLevelImage);
        
    mOverDriveBypassButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "OVERDRIVEBYPASS", mOverDriveBypassButton);
    setupButton(mOverDriveBypassButton, mOverDriveBypassButtonImage);
    
//    REVERB
    mReverbAmountImage.setNewRange(audioProcessor.apvts.getParameterRange("REVERBAMOUNT").start, audioProcessor.apvts.getParameterRange("REVERBAMOUNT").end);
    mReverbAmountSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "REVERBAMOUNT", mReverbAmountSlider);
    setupSlider(mReverbAmountSlider, mReverbAmountImage);

    mReverbHPCutoffImage.setNewRange(audioProcessor.apvts.getParameterRange("REVERBHIGHPASSFREQ").start, audioProcessor.apvts.getParameterRange("REVERBHIGHPASSFREQ").end);
    mReverbHPCutoffSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "REVERBHIGHPASSFREQ", mReverbHPCutoffSlider);
    setupSlider(mReverbHPCutoffSlider, mReverbHPCutoffImage);
    
    mReverbLPCutoffImage.setNewRange(audioProcessor.apvts.getParameterRange("REVERBLOWPASSFREQ").start, audioProcessor.apvts.getParameterRange("REVERBLOWPASSFREQ").end);
    mReverbLPCutoffSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "REVERBLOWPASSFREQ", mReverbLPCutoffSlider);
    setupSlider(mReverbLPCutoffSlider, mReverbLPCutoffImage);
    
    mReverbBypassButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "REVERBYPASS", mReverbBypassButton);
    setupButton(mReverbBypassButton, mReverbBypassButtonImage);
    
    setupButton(mReverbIncrementButton, mReverbIncrementButtonImage);
    mReverbIncrementButton.onClick = [this]() {mReverbSelector.setSelectedItemIndex(mReverbSelector.getSelectedItemIndex() + 1);};
    
    setupButton(mReverbDecrementButton, mReverbDecrementButtonImage);
    mReverbDecrementButton.onClick = [this]() { if (mReverbSelector.getSelectedItemIndex() > 0) {mReverbSelector.setSelectedItemIndex(mReverbSelector.getSelectedItemIndex() - 1);}};

    mReverbSelector.addItemList({"Spacious Hall","Short Slappy Room", "Dark Room", "Lush Plate", "Spring", "Stero Spread Verb" }, 1);
    mReverbSelectorAttachement = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "REVERBSELECTOR", mReverbSelector);
    addAndMakeVisible(mReverbSelectorImage);
    
      addAndMakeVisible(mReverbSelector);
    
    
//    DELAY (Amount, Feedback, LP, HP, Bypass, PingPong
    mDelayAmountImage.setNewRange(audioProcessor.apvts.getParameterRange("DELAYAMOUNT").start, audioProcessor.apvts.getParameterRange("DELAYAMOUNT").end);
    mDelayAmountSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DELAYAMOUNT", mDelayAmountSlider);
    setupSlider(mDelayAmountSlider, mDelayAmountImage);
    
    mDelayTimeImage.setNewRange(audioProcessor.apvts.getParameterRange("DELAYTIME").start, audioProcessor.apvts.getParameterRange("DELAYTIME").end);
    mDelayTimeSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DELAYTIME", mDelayTimeSlider);
    setupSlider(mDelayTimeSlider, mDelayTimeImage);

    
    mDelayFeedbackImage.setNewRange(audioProcessor.apvts.getParameterRange("DELAYFEEDBACK").start, audioProcessor.apvts.getParameterRange("DELAYFEEDBACK").end);
    mDelayFeedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DELAYFEEDBACK", mDelayFeedbackSlider);
    setupSlider(mDelayFeedbackSlider, mDelayFeedbackImage);
    
    mDelayHPFreqImage.setNewRange(audioProcessor.apvts.getParameterRange("DELAYHIGHPASSFREQ").start, audioProcessor.apvts.getParameterRange("DELAYHIGHPASSFREQ").end);
    mDelayHPFreqSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DELAYHIGHPASSFREQ", mDelayHPFreqSlider);
    setupSlider(mDelayHPFreqSlider, mDelayHPFreqImage);

    
    mDelayLPFreqImage.setNewRange(audioProcessor.apvts.getParameterRange("DELAYLOWPASSFREQ").start, audioProcessor.apvts.getParameterRange("DELAYLOWPASSFREQ").end);
    mDelayLPFreqSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DELAYLOWPASSFREQ", mDelayLPFreqSlider);
    setupSlider(mDelayLPFreqSlider, mDelayLPFreqImage);
    
    
    mDelayWidthImage.setNewRange(audioProcessor.apvts.getParameterRange("DELAYWIDTH").start, audioProcessor.apvts.getParameterRange("DELAYWIDTH").end);
    mDelayWidthSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DELAYWIDTH", mDelayWidthSlider);
    setupSlider(mDelayWidthSlider, mDelayWidthImage);
    
    mDelayBypassButtonAttachement = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "DELAYBYPASS", mDelayBypassButton);
    setupButton(mDelayBypassButton, mDelayBypassButtonImage);
    
    mDelaySnapToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "DELAYSNAP", mDelaySnapToggle);
    setupButton(mDelaySnapToggle, mDelaySnapToggleImage);
    
    mDelayTimeSnappedImage.setNewRange(audioProcessor.apvts.getParameterRange("DELAYDIVISION").start, audioProcessor.apvts.getParameterRange("DELAYDIVISION").end);
    mDelayTimeSnappedSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DELAYDIVISION", mDelayTimeSnappedSlider);
    setupSlider(mDelayTimeSnappedSlider, mDelayTimeSnappedImage);
    
    mDelayTimeSnappedSlider.textFromValueFunction = [this](double value)
    {
        switch (static_cast<int>(value)) {
            case 12:
                return "1/1";
                break;
            case 11:
                return "1/2D";
            case 10:
                return "1/2";
            case 9:
                return "1/4D";
            case 8:
                return "1/2T";
            case 7:
                return "1/4";
            case 6:
                return "1/8D";
            case 5:
                return "1/4T";
            case 4:
                return "1/8";
            case 3:
                return "1/16D";
            case 2:
                return "1/8T";
            case 1:
                return "1/16";
            case 0:
                return "1/16T";
                
            default:
                return "n/a";
                break;
        }
    };
    

    // LEVEL METER
    for (int i = 0; i < 2; i++)
    {
        addAndMakeVisible(preAmpLevelMeters[i]);
        addAndMakeVisible(postAmpLevelMeters[i]);
    }
    
    
    
    //    OSC MAPPING ----------------------------------------------------------------------------------------------------------------------------
    
    for (juce::String path : oscPaths) {
        addListener(this,path);
        DBG(path);
    }
//    connect the osc port
    connectOSC();

    auto ipaddress = juce::IPAddress::getLocalAddress().toString();
    
    DBG(ipaddress);
    
    accelXSmoothed.reset(15);
    
//    Preset manager
    addAndMakeVisible(presetPanel);
    presetPanel.setAlwaysOnTop(true);
    
//    Osc Manager
    addAndMakeVisible(oscManager);
    
    setSize (1000, 563);
    
    
}

GuitarOscAudioProcessorEditor::~GuitarOscAudioProcessorEditor()
{
    mOverDriveAmountSlider.setLookAndFeel(nullptr);
    mOverDriveLevelSlider.setLookAndFeel(nullptr);
    mOverDriveToneSlider.setLookAndFeel(nullptr);
    mOverDriveBiasSlider.setLookAndFeel(nullptr);
}

//==============================================================================
void GuitarOscAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    if (currentPage == 1)
    {
        g.drawImageWithin(page1Image, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::stretchToFit);
        repaint();
    }
    
    if (currentPage == 2)
    {
        g.drawImageWithin(page2Image, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::stretchToFit);
        repaint();
    }
    
    setVisibility(currentPage);
    
    for (int i = 0; i < 2; i++)
    {
        preAmpLevelMeters[i].setLevel(audioProcessor.preAmpRmsLevelsSmoothed[i].getCurrentValue());
        preAmpLevelMeters[i].repaint();
        postAmpLevelMeters[i].setLevel(audioProcessor.postAmpRmsLevelsSmoothed[i].getCurrentValue());
        postAmpLevelMeters[i].repaint();
    }
    
    if (*audioProcessor.apvts.getRawParameterValue("DELAYSNAP"))
    {
        mDelayTimeSnappedSlider.setEnabled(true);
        mDelayTimeSlider.setEnabled(false);
        mDelayTimeSnappedSlider.setPopupDisplayEnabled(true, false, this);
    } else
    {
        mDelayTimeSnappedSlider.setEnabled(false);
        mDelayTimeSnappedSlider.setVisible(false);
        mDelayTimeSnappedImage.setVisible(false);
        mDelayTimeSlider.setEnabled(true);
        mDelayTimeSnappedSlider.setPopupDisplayEnabled(false, false, this);
    }
    
    
}

void GuitarOscAudioProcessorEditor::setVisibility(int pageId) {
    
//    TODO: Remove the boolean visible!!
    
    bool visible = pageId == 1 || pageId == 2 || pageId == 3;
    mInputGainSlider.setVisible(pageId == 1);
    mAmpInputGainImage.setVisible(visible && pageId == 1);
    mBassGainSlider.setVisible(visible && pageId == 1);
    mBassGainImage.setVisible(visible && pageId == 1);
    mMidGainSlider.setVisible(visible && pageId == 1);
    mMidGainImage.setVisible(visible && pageId == 1);
    mTrebleGainSlider.setVisible(visible && pageId == 1);
    mTrebleGainImage.setVisible(visible && pageId == 1);
    mAmpBypassButton.setVisible(visible && pageId == 1);
    mAmpBypassButtonImage.setVisible(visible && pageId == 1);
    mPlus15DbButton.setVisible(visible && pageId == 1);
    mPlus15DbButtonImage.setVisible(visible && pageId == 1);
    mHaasDelaySlider.setVisible(visible && pageId == 1);
    mDoublerWidthImage.setVisible(visible && pageId == 1);
    mHaasBypassButton.setVisible(visible && pageId == 1);
    mDoublerBypassButtonImage.setVisible(visible && pageId == 1);
    mHaasSideButton.setVisible(visible && pageId == 1);
    mDoublerSideBypassButtonImage.setVisible(visible && pageId == 1);
    mGlobalOutputGainSlider.setVisible(visible && pageId == 1);
    mOutputGainImage.setVisible(visible && pageId == 1);

    mOverDriveAmountSlider.setVisible(visible && pageId == 2);
    mOverDriveAmountImage.setVisible(visible && pageId == 2);
    mOverDriveBiasSlider.setVisible(visible && pageId == 2);
    mOverDriveBiasImage.setVisible(visible && pageId == 2);
    mOverDriveToneSlider.setVisible(visible && pageId == 2);
    mOverDriveToneImage.setVisible(visible && pageId == 2);
    mOverDriveLevelSlider.setVisible(visible && pageId == 2);
    mOverDriveLevelImage.setVisible(visible && pageId == 2);
    mOverDriveBypassButton.setVisible(visible && pageId == 2);
    mOverDriveBypassButtonImage.setVisible(visible && pageId == 2);
    mReverbAmountSlider.setVisible(visible && pageId == 2);
    mReverbAmountImage.setVisible(visible && pageId == 2);
    mReverbHPCutoffSlider.setVisible(visible && pageId == 2);
    mReverbHPCutoffImage.setVisible(visible && pageId == 2);
    mReverbLPCutoffSlider.setVisible(visible && pageId == 2);
    mReverbLPCutoffImage.setVisible(visible && pageId == 2);
    mReverbBypassButton.setVisible(visible && pageId == 2);
    mReverbBypassButtonImage.setVisible(visible && pageId == 2);
//    mReverbSelector.setVisible(visible && pageId == 2);
    mReverbSelectorImage.setVisible(visible && pageId == 2);
    mReverbIncrementButton.setVisible(visible && pageId == 2);
    mReverbIncrementButtonImage.setVisible(visible && pageId == 2);
    mReverbDecrementButton.setVisible(visible && pageId == 2);
    mReverbDecrementButtonImage.setVisible(visible && pageId == 2);
    mDelayAmountSlider.setVisible(visible && pageId == 2);
    mDelayAmountImage.setVisible(visible && pageId == 2);
    mDelayFeedbackSlider.setVisible(visible && pageId == 2);
    mDelayFeedbackImage.setVisible(visible && pageId == 2);
    mDelayHPFreqSlider.setVisible(visible && pageId == 2);
    mDelayHPFreqImage.setVisible(visible && pageId == 2);
    mDelayLPFreqSlider.setVisible(visible && pageId == 2);
    mDelayLPFreqImage.setVisible(visible && pageId == 2);
    mDelayBypassButton.setVisible(visible && pageId == 2);
    mDelayBypassButtonImage.setVisible(visible && pageId == 2);
    mDelayTimeSlider.setVisible(visible && pageId == 2);
    mDelayTimeImage.setVisible(visible && pageId == 2);
    mDelayWidthSlider.setVisible(visible && pageId == 2);
    mDelayWidthImage.setVisible(visible && pageId == 2);
    mDelaySnapToggle.setVisible(visible && pageId == 2);
    mDelaySnapToggleImage.setVisible(visible && pageId == 2);
    mDelayTimeSnappedSlider.setVisible(visible && pageId == 2);
    mDelayTimeSnappedImage.setVisible(visible && pageId == 2);
}


void GuitarOscAudioProcessorEditor::resized()
{
//    mPageSelector.setBounds(12, 9, 86, 28);
    auto pageSelectorSpacing = 45;
    auto size = 55;
    auto pageSelectorX = 4;
    auto pageSelectorY = 0;
    
//        int imageXpos = JUCE_LIVE_CONSTANT(352);
//        int imageYpos = JUCE_LIVE_CONSTANT(282);
//        int radius = JUCE_LIVE_CONSTANT(60);
    mPageSelectors[0].setBounds(pageSelectorX, pageSelectorY, size,size);
    mPageSelectors[1].setBounds(pageSelectorX + pageSelectorSpacing, pageSelectorY, size,size);
    
    mInfoButton.setBounds(968,11, 23,23);


    
    mAmpBypassButton.setBounds(222, 274, 30, 30);
    mAmpBypassButtonImage.setBounds(217, 274, scaleFactor * ampBypassButtonWidth, scaleFactor * ampBypassButtonHeight);
    mPlus15DbButton.setBounds(222, 297, 30, 30);
    mPlus15DbButtonImage.setBounds(216, 296, scaleFactor * plus15DbButtonWidth, scaleFactor * plus15DbButtonHeight);
    mInputGainSlider.setBounds(255, 276, 60, 60);
    mAmpInputGainImage.setBounds(257, 281, scaleFactor * ampInputGainWidth, scaleFactor * ampInputGainHeight);
    mBassGainSlider.setBounds(342, 276, 60, 60);
    mBassGainImage.setBounds(345, 283, scaleFactor * bassGainWidth, scaleFactor * bassGainHeight);
    mMidGainSlider.setBounds(428,278, 60, 60);
    mMidGainImage.setBounds(434, 284, scaleFactor * midGainWidth, scaleFactor * midGainHeight);
    mTrebleGainSlider.setBounds(515, 278, 60, 60);
    mTrebleGainImage.setBounds(514, 284, scaleFactor * trebleGainWidth, scaleFactor * trebleGainHeight);
    mGlobalOutputGainSlider.setBounds(602, 277, 60, 60);
    mOutputGainImage.setBounds(605, 285, scaleFactor * outputGainWidth, scaleFactor * OutputGainHeight);
    mHaasDelaySlider.setBounds(752,279, 60, 60);
    mDoublerWidthImage.setBounds(757, 270, scaleFactor * doublerWidthWidth, doublerWidthHeight);
    mHaasBypassButton.setBounds(729,278, 30, 30);
    mDoublerBypassButtonImage.setBounds(726.5, 278, scaleFactor * doublerBypassButtonWidth, scaleFactor * doublerBypassButtonHeight);
    mHaasSideButton.setBounds(728,301, 30, 30);
    mDoublerSideBypassButtonImage.setBounds(726, 299, scaleFactor * doublerSideButtonWidth, scaleFactor * doublerSideButtonHeight);

    auto scaledWidth = scaleFactor * pedalKnobWidth;
    auto scaledHeight = scaleFactor * pedalKnobHeight;
    
    mOverDriveAmountSlider.setBounds(64,92,66,66);
    mOverDriveAmountImage.setBounds(66, 92, scaleFactor * pedalKnobWidth, scaleFactor * pedalKnobHeight);
    mOverDriveBiasSlider.setBounds(63, 222, 66, 66);
    mOverDriveBiasImage.setBounds(66, 222, scaleFactor * pedalKnobWidth, scaleFactor * pedalKnobHeight);

    mOverDriveToneImage.setBounds(160.5, 222, scaleFactor * pedalKnobWidth, scaleFactor * pedalKnobHeight);
    mOverDriveToneSlider.setBounds(157, 223, 66, 66);
    mOverDriveLevelImage.setBounds(160, 92, scaleFactor * pedalKnobWidth, scaleFactor * pedalKnobHeight);
    mOverDriveLevelSlider.setBounds(157, 91, 66, 66);
    mOverDriveBypassButtonImage.setBounds(59.5, 302, scaleFactor * pedalBypassButtonWidth, scaleFactor * pedalBypassButtonHeight);
    mOverDriveBypassButton.setBounds(118, 328, 50, 50);

    mReverbAmountSlider.setBounds(455, 192, 66, 66);
    mReverbAmountImage.setBounds(457, 193, scaleFactor * pedalKnobWidth, scaleFactor * pedalKnobHeight);
    mReverbHPCutoffSlider.setBounds(410, 94, 66, 66);
    mReverbHPCutoffImage.setBounds(413, 95, scaleFactor * pedalKnobWidth, scaleFactor * pedalKnobHeight);
    mReverbLPCutoffSlider.setBounds(503, 95, 65, 65);
    mReverbLPCutoffImage.setBounds(504, 95, scaleFactor * pedalKnobWidth, scaleFactor * pedalKnobHeight);
//    mReverbSelector.setBounds(50, 50, 80, 20);
    mReverbSelectorImage.setBounds(453, 294, scaleFactor * reverbSelectorWidth, reverbSelectorHeight);
    mReverbIncrementButton.setBounds(526, 290, 21, 21);
    mReverbIncrementButtonImage.setBounds(525.5, 289, scaleFactor * reverbIncrementWidth, scaleFactor * reverbIncrementHeight);
    mReverbDecrementButton.setBounds(428, 290, 21, 21);
    mReverbDecrementButtonImage.setBounds(425, 289, scaleFactor * reverbIncrementWidth, scaleFactor * reverbIncrementHeight);
    mReverbBypassButton.setBounds(464, 325, 50, 50);
    mReverbBypassButtonImage.setBounds(406, 302, scaleFactor * pedalBypassButtonWidth, scaleFactor * pedalBypassButtonHeight);

//    auto x = JUCE_LIVE_CONSTANT(749);
//    auto y = JUCE_LIVE_CONSTANT(300);
    
    mDelayAmountSlider.setBounds(817, 178, 66,66);
    mDelayAmountImage.setBounds(820, 177, scaledWidth, scaledHeight);
    mDelayFeedbackSlider.setBounds(817, 89, 66,66);
    mDelayFeedbackImage.setBounds(820, 90, scaledWidth, scaledHeight);
    mDelayHPFreqSlider.setBounds(751,90,66,66);
    mDelayHPFreqImage.setBounds(754, 90, scaledWidth, scaledHeight);
    mDelayLPFreqSlider.setBounds(883,89,66,66);
    mDelayLPFreqImage.setBounds(886, 90, scaledWidth, scaledHeight);
    mDelayTimeSlider.setBounds(759, 235, 66, 66);
    mDelayTimeImage.setBounds(762, 235, scaledWidth, scaledHeight);
    mDelayWidthSlider.setBounds(870,235, 66, 66);
    mDelayWidthImage.setBounds(873, 235, scaledWidth, scaledHeight);
    mDelayBypassButton.setBounds(833, 329, 50, 50);
    mDelayBypassButtonImage.setBounds(768, 302, scaleFactor * pedalBypassButtonWidth, scaleFactor * pedalBypassButtonHeight);
    mDelaySnapToggle.setBounds(748, 294, 21, 21);
    mDelaySnapToggleImage.setBounds(753, 274, delaySnapToggleWidth, delaySnapToggleHeight);
    mDelayTimeSnappedSlider.setBounds(759, 235, 66, 66);
    mDelayTimeSnappedImage.setBounds(762, 235, scaledWidth, scaledHeight);

//
//    accelXmappingSelector.setBounds(100, 100, 100, 20);
//    accelXScaleFactor.setBounds(200, 100, 100, 20);
//    mOSCBypassButton.setBounds(200, 150, 50, 50);
//    ipPort.setBounds(100, 200, 100, 30);
    
    auto spacing = 13;
    auto preInitalPos = 11;
    auto levelY = 60;
    preAmpLevelMeters[0].setBounds(preInitalPos, levelY, 10, 100);
    preAmpLevelMeters[1].setBounds(preInitalPos + spacing, levelY, 10, 100);
    
    auto postInitialPos = 968;
    postAmpLevelMeters[0].setBounds(postInitialPos, levelY, 10, 100);
    postAmpLevelMeters[1].setBounds(postInitialPos + spacing, levelY, 10, 100);
    
    
//    Preset manager
    const auto container = juce::Rectangle<int>(getWidth()/2 - 250,0,500,45);
           
    oscManager.setBounds(getLocalBounds());
    presetPanel.setBounds(container);
    //    needs to be painted over
}

void GuitarOscAudioProcessorEditor::setLabelProperties(const juce::String &name, juce::Label &label)
{
    addAndMakeVisible (label);
    label.setText (name, juce::dontSendNotification);
    label.setColour (juce::Label::textColourId, juce::Colours::orange);
    label.setJustificationType (juce::Justification::bottomRight);
}

//Callback function implementation
void GuitarOscAudioProcessorEditor::oscMessageReceived(const juce::OSCMessage &message)
{
    if (oscManager.getOscStatus())
    {
        float currentMessage;
        if (message[0].isFloat32())
        {
            currentMessage = message[0].getFloat32();
        }
        else if (message[0].isInt32())
        {
            currentMessage = message[0].getInt32();
        } else {
            jassert("Unknown data Type for OSC message!");
        }
        
        // get the address of the message
        juce::String address = message.getAddressPattern().toString();
        
    //    if (message.size() == 1 && mOSCBypassButton.getToggleStateValue() == true)
        if (message.size() == 1)
        {
            for (int i = 0; i < oscPaths.size(); ++i)
            {
                if (address == oscPaths[i])
                {
                    switch (i)
                    {
                        case 0:
//                             Handle OSC message for "/AmpToggle"
                                 currentPage = currentMessage + 1; // +1 is there to handle the osc indexing
                            break;
                        case 1:
                            // Handle OSC message for "/AMP/InputGain"
                            mInputGainSlider.setValue(juce::jmap(currentMessage, audioProcessor.apvts.getParameterRange("INPUTGAIN").start,  audioProcessor.apvts.getParameterRange("INPUTGAIN").end));
                            break;
                        case 2:
                            // Handle OSC message for "/AMP/OutputGain"
                            mGlobalOutputGainSlider.setValue(juce::jmap(currentMessage, audioProcessor.apvts.getParameterRange("OUTPUTGAIN").start,  audioProcessor.apvts.getParameterRange("INPUTGAIN").end));
                            break;
                        case 3:
                            // Handle OSC message for "/AMP/BassGain"
                            mBassGainSlider.setValue(juce::jmap(currentMessage, audioProcessor.apvts.getParameterRange("BASSGAIN").start,  audioProcessor.apvts.getParameterRange("INPUTGAIN").end));
                            break;
                        case 4:
                            // Handle OSC message for "/AMP/MidGain"
                            mMidGainSlider.setValue(juce::jmap(currentMessage, audioProcessor.apvts.getParameterRange("MIDGAIN").start,  audioProcessor.apvts.getParameterRange("MIDGAIN").end));
                            break;
                        case 5:
                            // Handle OSC message for "/AMP/TrebleGain"
                            mTrebleGainSlider.setValue(juce::jmap(currentMessage, audioProcessor.apvts.getParameterRange("TREBLEGAIN").start,  audioProcessor.apvts.getParameterRange("TREBLEGAIN").end));
                            break;
                        case 6:
                            // Handle OSC message for "/AMP/DoublerWidth"
                            mHaasDelaySlider.setValue(juce::jmap(currentMessage, audioProcessor.apvts.getParameterRange("HAASDELAY").start,  audioProcessor.apvts.getParameterRange("HAASDELAY").end));
                            mHaasDelaySlider.setValue(juce::jmap(currentMessage,0.0f,60.0f));
                            break;
                        case 7:
                            // Handle OSC message for "/FX/OverdriveBypass"
    //                        mOverDriveBypassButton.setToggleState(currentMessage, juce::NotificationType::dontSendNotification);
                            mOverDriveBypassButton.triggerClick();
                            break;
                        case 8:
                            // Handle OSC message for "/FX/InputGain"
                            mOverDriveAmountSlider.setValue(juce::jmap(currentMessage, audioProcessor.apvts.getParameterRange("OVERDRIVEAMOUNT").start,  audioProcessor.apvts.getParameterRange("OVERDRIVEAMOUNT").end));
                            break;
                        case 9:
                            // Handle OSC message for "/FX/OutputGain"
                            mOverDriveLevelSlider.setValue(juce::jmap(currentMessage, -30.0f, 1.0f));
                            break;
                        case 10:
                            // Handle OSC message for "/FX/Bias"
                            mOverDriveBiasSlider.setValue(juce::jmap(currentMessage, audioProcessor.apvts.getParameterRange("OVERDRIVEBIAS").start,  audioProcessor.apvts.getParameterRange("OVERDRIVEBIAS").end));
                            break;
                        case 11:
                            // Handle OSC message for "/FX/Tone"
                            mOverDriveToneSlider.setValue(juce::jmap(currentMessage, audioProcessor.apvts.getParameterRange("OVERDRIVETONE").start,  audioProcessor.apvts.getParameterRange("OVERDRIVETONE").end));
                            break;
                        case 12:
                            // Handle OSC message for "/FX/ReverbBypass"
                            mReverbBypassButton.setToggleState(currentMessage, juce::NotificationType::dontSendNotification);
                            break;
                        case 13:
                            // Handle OSC message for "/FX/ReverbMix"
                            mReverbAmountSlider.setValue(juce::jmap(currentMessage, audioProcessor.apvts.getParameterRange("REVERBAMOUNT").start,  audioProcessor.apvts.getParameterRange("REVERBAMOUNT").end));
                            break;
                        case 14:
                            // Handle OSC message for "/FX/ReverbHPF"
                            mReverbHPCutoffSlider.setValue(juce::jmap(currentMessage, audioProcessor.apvts.getParameterRange("REVERBHIGHPASSFREQ").start,  audioProcessor.apvts.getParameterRange("REVERBHIGHPASSFREQ").end));
                            break;
                        case 15:
                            // Handle OSC message for "/FX/ReverbLPF"
                            mReverbLPCutoffSlider.setValue(juce::jmap(currentMessage, audioProcessor.apvts.getParameterRange("REVERBLOWPASSFREQ").start,  audioProcessor.apvts.getParameterRange("REVERBLOWPASSFREQ").end));
                            break;
                        case 16:
                            // Handle OSC message for "/FX/ReverbSelector"
                            mReverbSelector.setSelectedItemIndex(currentMessage);
                            break;
                        case 17:
                            // Handle OSC message for "/FX/DelayBypass"
                            mDelayBypassButton.setToggleState(currentMessage, juce::NotificationType::dontSendNotification);
                            break;
                        case 18:
                            // Handle OSC message for "/FX/DelayMix"
                            mDelayAmountSlider.setValue(juce::jmap(currentMessage, audioProcessor.apvts.getParameterRange("DELAYAMOUNT").start,  audioProcessor.apvts.getParameterRange("DELAYAMOUNT").end));
                            break;
                        case 19:
                            // Handle OSC message for "/FX/DelayHPF"
                            mDelayHPFreqSlider.setValue(juce::jmap(currentMessage, audioProcessor.apvts.getParameterRange("DELAYHIGHPASSFREQ").start,  audioProcessor.apvts.getParameterRange("DELAYHIGHPASSFREQ").end));
                            break;
                        case 20:
                            // Handle OSC message for "/FX/DelayLPF"
                            mDelayLPFreqSlider.setValue(juce::jmap(currentMessage, audioProcessor.apvts.getParameterRange("DELAYLOWPASSFREQ").start,  audioProcessor.apvts.getParameterRange("DELAYLOWPASSFREQ").end));
                            break;
                        case 21:
                            // Handle OSC message for "/FX/DelayTime"
                            // Checking to see whether snapped slider is enabled or not
                            if (mDelaySnapToggle.getToggleState())
                            {
                                mDelayTimeSnappedSlider.setValue(juce::jmap(currentMessage, audioProcessor.apvts.getParameterRange("DELAYDIVISION").start,  audioProcessor.apvts.getParameterRange("DELAYDIVISION").end));
                                break;
                            }
                            mDelayTimeSlider.setValue(juce::jmap(currentMessage, audioProcessor.apvts.getParameterRange("DELAYTIME").start,  audioProcessor.apvts.getParameterRange("DELAYTIME").end));
                            break;
                        case 22:
//                             Handle OSC message for "/FX/DelaySpread"
                            mDelayWidthSlider.setValue(juce::jmap(currentMessage, audioProcessor.apvts.getParameterRange("DELAYWIDTH").start,  audioProcessor.apvts.getParameterRange("DELAYWIDTH").end));
                            break;
                        case 23:
                            // Handle OSC message for "/FX/DelayFeedback"
                            mDelayFeedbackSlider.setValue(juce::jmap(currentMessage, audioProcessor.apvts.getParameterRange("DELAYFEEDBACK").start,  audioProcessor.apvts.getParameterRange("DELAYFEEDBACK").end));
//                            FIX
                        case 24:
                            // Handle OSC message for "/AMP/CabBypass"
                            mAmpBypassButton.setToggleState(currentMessage, juce::NotificationType::dontSendNotification);
                            break;
                        case 25:
                            // Handle OSC message for "/AMP/Plus15Db"
                            mPlus15DbButton.setToggleState(currentMessage, juce::NotificationType::dontSendNotification);
                            break;
                        case 26:
                            // Handle OSC message for "/AMP/DoublerBypass"
                            mHaasBypassButton.setToggleState(currentMessage, juce::NotificationType::dontSendNotification);
                            break;
                        case 27:
                            // Handle OSC message for "/AMP/DoublerSide"
                            mHaasSideButton.setToggleState(currentMessage, juce::NotificationType::dontSendNotification);
                            break;
                        case 28:
                            // Handle OSC message for "/AMP/PresetIncrementButton"
                            if (currentMessage == 1)
                            {
                                presetPanel.increment();
                            }
                            break;
                        case 29:
                            // Handle OSC message for "/AMP/PresetDecrementButton"
                            if (currentMessage == 1)
                            {
                                presetPanel.decrement();
                            }
                            break;
                        case 30:
                            // Handle OSC message for "/FX/TempoSyncToggle"
                            mDelaySnapToggle.triggerClick();
                            break;
                        default:
                            break;
                    }
                    break;
                }
            }
        }
    }
//    DBG(accelXSmoothed.getNextValue());
}

void GuitarOscAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Component& image)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 50, 25);
    slider.setAlwaysOnTop(true);
    addAndMakeVisible(slider);
    slider.setAlpha(0.0f);
    addAndMakeVisible(image);
}

void GuitarOscAudioProcessorEditor::setupButton(juce::Button& button, juce::Component& image)
{
    addAndMakeVisible(button);
    button.setClickingTogglesState(true);
    button.setAlwaysOnTop(true);
    button.setAlpha(0.0f);
    addAndMakeVisible(image);
}

void GuitarOscAudioProcessorEditor::connectOSC()
{
    disconnect();
    connect(oscManager.getPortNumber());
    currentOSCNumber = oscManager.getPortNumber();
}
