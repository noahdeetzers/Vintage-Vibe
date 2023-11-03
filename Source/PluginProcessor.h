/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PresetManager.h"

//==============================================================================
/**
*/
class GuitarOscAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    GuitarOscAudioProcessor();
    ~GuitarOscAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    
    juce::AudioProcessorValueTreeState apvts;
    
    void updatePreEq();
    void updatePostEq();
    void updateToneControl();
    void updateReverbEq();
    void updateDelayEq();

    void reset() override ;
    
    int irNumber;
    int irSize;
    const char* irData;
    int currentLoadedIr;
    void loadIR();
    

    juce::AudioBuffer<float> temp;
    
    //Level meter
    
    std::array<float, 2> preAmpRmsLevels {};
//    float preAmpRmsLevels [2] {0.0f,0.0f};
    std::array<juce::LinearSmoothedValue<float>, 2> preAmpRmsLevelsSmoothed {};
//    juce::LinearSmoothedValue<float> preAmpRmsLevelsSmoothed [2] {};
    
    std::array<float, 2> postAmpRmsLevels {};
//    float postAmpRmsLevels [2] {0.0f,0.0f};
    
    std::array<juce::LinearSmoothedValue<float>, 2> postAmpRmsLevelsSmoothed {};
//    juce::LinearSmoothedValue<float> postAmpRmsLevelsSmoothed [2] {};
    
//    Preset manager
    
    PresetManager& getPresetManager() { return *presetManager;};

//    get snapped bpm value
    double getBPM();
    void setBPM(double bpm);
    double convertDelayToSnappedVal(int delVal);
    juce::String selected = juce::String("");
    
    float lerp(float* buffer, int size, float idx);
    
private:
//    This is the amp's built in eq curve created from trying to tone match the default Neural DSP Rabea Archetype amp
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> parametric1;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> parametric2;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> parametric3;
    
//    post amp eq curve that can be controlled via sliders
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> bassFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> midFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> trebleFilter;
    
//    smoothed values for the amp
    juce::SmoothedValue<float> ampHeadSmoothedValues [6];
    
//    Convolution message queue - lets them operate on the same background thread
    juce::dsp::ConvolutionMessageQueue queue;
    
//    convolution object for IR
//    juce::dsp::Convolution cabIrConv {queue};
    juce::dsp::Convolution cabIrConv { juce::dsp::Convolution::NonUniform {2048}, queue}; // basaed off of the juce dsp settings https://github.com/juce-framework/JUCE/blob/master/examples/Plugins/DSPModulePluginDemo.h
    
//    OVERDRIVE
    juce::dsp::Gain<float> overDriveInputGain;
    juce::dsp::Bias<float> overDriveBias;
    juce::dsp::WaveShaper<float> fxOverdrive;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> toneFilter;
    juce::SmoothedValue<float,juce::ValueSmoothingTypes::Multiplicative> toneFilterSmoothed;
    juce::dsp::Gain<float> overDriveOutputGain;
    
//    REVERB
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> reverbLP;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> reverbHP;
    juce::SmoothedValue<float> reverbFilterSmoothed [2];
//    juce::dsp::Convolution reverbConv{ juce::dsp::Convolution::NonUniform {2048}};
//    juce::dsp::Convolution reverbConv{juce::dsp::Convolution::NonUniform {2048}, queue};
    juce::dsp::Convolution reverbConv{juce::dsp::Convolution::Latency {128}, queue};
//    juce::dsp::Convolution reverbConv{juce::dsp::Convolution::Latency {8192}, queue};
//    Head size could be partition size (or your literal head)
    

//    haas
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Thiran> haasDelay;
    
//    DELAY
    juce::dsp::DryWetMixer<float> delayMixer;
    juce::SmoothedValue<float> delayMixerSmoothed;
    juce::SmoothedValue<float> delayTimeSmoothed;
    
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayLeft;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayRight;
        
    juce::dsp::DryWetMixer<float> delayDryWetMixer;
    juce::dsp::DryWetMixer<float> delayWidthMixer;
    
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> delayLP;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> delayHP;
    juce::SmoothedValue<float> delayFilterSmoothed [2];
    
    void updateVolumeMeter(juce::AudioBuffer<float>& buffer, juce::SmoothedValue<float>* smoothedLevels, int numChannels);
    
//    BPM
    double beatsPerMinute {100};
    
//    DelayValues
    #include <map>

    std::map<std::string, float> delayTimes = {
        {"WholeNote", 1.0f},
        {"HalfNote", 0.5f},
        {"QuarterNote", 0.25f},
        {"EighthNote", 0.125f},
        {"SixteenthNote", 0.0625f},
        {"DottedHalfNote", 0.75f},
        {"DottedQuarterNote", 0.375f},
        {"DottedEighthNote", 0.1875f},
        {"DottedSixteenthNote", 0.09375f},
        {"TripletHalfNote", 1.0f / 3.0f},
        {"TripletQuarterNote", 1.0f / 6.0f},
        {"TripletEighthNote", 1.0f / 12.0f},
        {"TripletSixteenthNote", 1.0f / 24.0f}
    };
    
    struct DelayInfo {
        double sampleValue;
        std::string delayName;
    };
    
    juce::LagrangeInterpolator interpolator;
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    
    //    Create instance of the presetManager obj
     std::unique_ptr<PresetManager> presetManager;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarOscAudioProcessor)
};
