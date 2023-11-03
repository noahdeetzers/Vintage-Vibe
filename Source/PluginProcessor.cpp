/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GuitarOscAudioProcessor::GuitarOscAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
    apvts.state = juce::ValueTree("savedParams");

    apvts.state.setProperty(PresetManager::presetNameProperty, "", nullptr);
    apvts.state.setProperty("version", ProjectInfo::versionString, nullptr);

    presetManager = std::make_unique<PresetManager>(apvts);

}

GuitarOscAudioProcessor::~GuitarOscAudioProcessor()
{
}

//==============================================================================
const juce::String GuitarOscAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GuitarOscAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GuitarOscAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GuitarOscAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GuitarOscAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GuitarOscAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GuitarOscAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GuitarOscAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GuitarOscAudioProcessor::getProgramName (int index)
{
    return {};
}

void GuitarOscAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GuitarOscAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.numChannels = getTotalNumOutputChannels();
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    
//  IR convolution
    cabIrConv.loadImpulseResponse(BinaryData::cabIr_wav, BinaryData::cabIr_wavSize,
        juce::dsp::Convolution::Stereo::yes,
        juce::dsp::Convolution::Trim::no, BinaryData::cabIr_wavSize, juce::dsp::Convolution::Normalise::yes);

//    Update Eq's
    updatePreEq();
    updatePostEq();
    updateReverbEq();
    updateDelayEq();

//    Pre EQ
    parametric1.prepare(spec);
    parametric2.prepare(spec);
    parametric3.prepare(spec);
    
//    Post EQ
    bassFilter.prepare(spec);
    midFilter.prepare(spec);
    trebleFilter.prepare(spec);
    
//    Prepare cabinet convolution
    cabIrConv.prepare(spec);
    
//    OVERDRIVE Initialization
    overDriveInputGain.prepare(spec);
    overDriveInputGain.setRampDurationSeconds(0.5);
    
    overDriveBias.prepare(spec);
    overDriveBias.setRampDurationSeconds(0.5);
    
    fxOverdrive.prepare(spec);
    fxOverdrive.functionToUse = [] (float x)
                                {
                                    return std::tanh (x);
                                };
    toneFilter.prepare(spec);
    updateToneControl();
    overDriveOutputGain.prepare(spec);
    overDriveOutputGain.setRampDurationSeconds(0.5);
    
    reverbLP.prepare(spec);
    reverbHP.prepare(spec);
    reverbConv.prepare(spec);
    irData = BinaryData::reverb1_wav;
    irSize = BinaryData::reverb1_epicure 100’sepicure 100’swavSize;
    loadIR();
    
//    Haas
    haasDelay.prepare(spec);
    haasDelay.setMaximumDelayInSamples(sampleRate);
    
//    Delay
    delayMixer.prepare(spec);
    delayMixer.setMixingRule(juce::dsp::DryWetMixingRule::linear);
    delayLP.prepare(spec);
    delayHP.prepare(spec);

    temp.setSize(getTotalNumOutputChannels(), samplesPerBlock);
    
    //Level meter
    for (int i = 0; i < 2; i++)
    {
        preAmpRmsLevelsSmoothed[i].reset(sampleRate, 0.09);
        preAmpRmsLevelsSmoothed[i].setCurrentAndTargetValue(0);
        
        postAmpRmsLevelsSmoothed[i].reset(sampleRate, 0.09);
        postAmpRmsLevelsSmoothed[i].setCurrentAndTargetValue(0);
    }
    
    delayLeft.prepare(spec);
    delayLeft.setMaximumDelayInSamples(5.0f * sampleRate);
    delayLeft.reset();
    
    delayRight.prepare(spec);
    delayRight.setMaximumDelayInSamples(5.0f * sampleRate);
    delayRight.reset();
    
    delayDryWetMixer.prepare(spec);
    delayDryWetMixer.reset();
    
    delayWidthMixer.prepare(spec);
    delayWidthMixer.reset();

//    Reset all methods
    reset();
    
    interpolator.reset();
}

void GuitarOscAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    
//    mainProcessor->releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GuitarOscAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono())
    {
        // Mono-to-mono
        if (layouts.getMainInputChannelSet() == juce::AudioChannelSet::mono())
            return true;
    }
    if (layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo())
    {
        // Mono-to-stereo OR stereo-to-stereo
        if ((layouts.getMainInputChannelSet() == juce::AudioChannelSet::mono()) || (layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo()))
            return true;
    }
    return false;
}
#endif

void GuitarOscAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

//    Code to run pre-amp levelmeter
    
//    mono->stereo compatibility
    if (totalNumInputChannels == 1 && totalNumOutputChannels == 2)
    {
        buffer.copyFrom (1, 0, buffer.getReadPointer (0), buffer.getNumSamples());
    }
    
    //    Code to run pre-amp levelmeter
    updateVolumeMeter(buffer, preAmpRmsLevelsSmoothed.data(), 2);
    
    buffer.applyGain(juce::Decibels::decibelsToGain(18.0f));
    if (*apvts.getRawParameterValue("PLUS15DB"))
    {
        buffer.applyGain(juce::Decibels::decibelsToGain(15.0f));
    }
    
    auto inputGain = juce::Decibels::decibelsToGain((float) *apvts.getRawParameterValue("INPUTGAIN"));
    ampHeadSmoothedValues[0].setTargetValue(inputGain);

    buffer.applyGain(ampHeadSmoothedValues[0].getNextValue());
    buffer.applyGain(inputGain);

    auto audioBlock = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(audioBlock);

    parametric1.process(context);
    parametric2.process(context);
    parametric3.process(context);

//    PRE EFFECTS PROCESSING
    overDriveInputGain.setGainDecibels(*apvts.getRawParameterValue("OVERDRIVEAMOUNT"));
    overDriveOutputGain.setGainDecibels(*apvts.getRawParameterValue("OVERDRIVELEVEL"));
    overDriveBias.setBias(*apvts.getRawParameterValue("OVERDRIVEBIAS"));

    updateToneControl();
    if (*apvts.getRawParameterValue("OVERDRIVEBYPASS") == true)
    {
        overDriveInputGain.process(context);
        overDriveBias.process(context);
        fxOverdrive.process(context);
        toneFilter.process(context);
        overDriveOutputGain.process(context);
    }

    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {

//            channelData[sample] *= *apvts.getRawParameterValue("INPUTGAIN");
//            double inputG = JUCE_LIVE_CONSTANT(1.0f);
//            channelData[sample] *= inputG;

//            https://www.youtube.com/watch?v=2fb47zQdLy0 - this is the source for the asymptotic limit function
//            allows for a lot of gain before hard clipping (going to be used for main tube distortion stage)
            channelData[sample] = channelData[sample] / (abs(channelData[sample]) + 1);
        }

    }

    updatePostEq();
    bassFilter.process(context);
    midFilter.process(context);
    trebleFilter.process(context);

    if (*apvts.getRawParameterValue("CABYPASS") == false)
    {
        cabIrConv.process(context);
    }

//// DELAY ---------------------------------------------------------------------------------------------------------
    
    float delayTime = *apvts.getRawParameterValue("DELAYTIME");
    
    if (getPlayHead() != nullptr && *apvts.getRawParameterValue("DELAYSNAP") == true)
    {
        setBPM(*getPlayHead()->getPosition()->getBpm());
    
        delayTime = convertDelayToSnappedVal(*apvts.getRawParameterValue("DELAYDIVISION"));
//        DBG(*apvts.getRawParameterValue("DELAYDIVISION"));
//        DBG(selected);
    }
        delayTimeSmoothed.setTargetValue(delayTime);
//        delayLeft.setDelay(delayTimeSmoothed.getNextValue());
//        delayRight.setDelay(delayTimeSmoothed.getNextValue()  * 2);

        float feedback = *apvts.getRawParameterValue("DELAYFEEDBACK") / 100.0f;
        float delayWidth = *apvts.getRawParameterValue("DELAYWIDTH") / 100.0f;

    updateDelayEq();
    
    bool cleared = false;
    if (*apvts.getRawParameterValue("DELAYBYPASS") == true)
    {
        delayDryWetMixer.pushDrySamples(audioBlock);
        delayHP.process(context);
        delayLP.process(context);
        
        if (totalNumOutputChannels == 2)
        {
            auto* channelDataL = buffer.getWritePointer(0);
            auto* channelDataR = buffer.getWritePointer(1);
            
            for (int sample = 0; sample < buffer.getNumSamples(); sample++)
            {
                delayLeft.setDelay(delayTimeSmoothed.getNextValue());
                delayRight.setDelay(delayTimeSmoothed.getNextValue()  * 2);
                
                // Get delayed samples from left and right delay lines
                auto delayedLeft = delayLeft.popSample(0);
                auto delayedRight = delayRight.popSample(1);
                
//                delayedLeft = lerp(channelDataL, buffer.getNumSamples(), sample);
//                delayedRight = lerp(channelDataR, buffer.getNumSamples(), sample);

                // Apply feedback to the delayed samples
                delayedLeft *= feedback;
                delayedRight *= feedback;

                // Mix delayed and input samples for ping pong effect
                channelDataL[sample] += delayedLeft;
                channelDataR[sample] += delayedRight;
    
                // Push input samples to the appropriate delay lines
                delayLeft.pushSample(0, channelDataR[sample]);
                delayRight.pushSample(1, channelDataL[sample]);

                // calculate scale coefficient
                auto coef_S = delayWidth;

                // apply mid-side processing
                auto m = (channelDataL[sample] + channelDataR[sample]) * 0.5;
                auto s = (channelDataR[sample] - channelDataL[sample]) * coef_S;

                channelDataL[sample] = m - s;
                channelDataR[sample] = m + s;
            }
        }
        else if (totalNumOutputChannels == 1)
        {
            for (int sample = 0; sample < buffer.getNumSamples(); sample++)
            {
                
                
                auto* channelData = buffer.getWritePointer(0);
                
                channelData[sample] = lerp(channelData, buffer.getNumSamples(), sample);
                // Get delayed sample from left delay line
                auto delayed = delayLeft.popSample(0);

                // Apply feedback to the delayed sample
                delayed *= feedback;

                
                // Mix delayed and input samples
                channelData[sample] += delayed;


                // Push input sample to the delay line
                delayLeft.pushSample(0, channelData[sample]);
                
                
                
            }
        }

        delayDryWetMixer.mixWetSamples(audioBlock);
        delayDryWetMixer.setWetMixProportion(*apvts.getRawParameterValue("DELAYAMOUNT") / 100.0f);
    }
    else if (cleared == false)
    {
        delayLeft.reset();
        delayRight.reset();
        cleared = true;
    }

    //Getting number and Size of IR
    irNumber = *apvts.getRawParameterValue("REVERBSELECTOR");
    juce::String irName = BinaryData::namedResourceList[irNumber];
//    DBG(irName);
    irData = BinaryData::getNamedResource(irName.toRawUTF8(), irSize);

    audioBlock.copyTo(temp);
    if (*apvts.getRawParameterValue("REVERBYPASS") == true)
    {
        // Update IR if it has been changed in GUI
        if (irNumber != currentLoadedIr)
        {
//            DBG(irNumber);
//            DBG(currentLoadedIr);
            loadIR();
            currentLoadedIr = irNumber;
        }

        updateReverbEq();
        reverbConv.process(context);
        reverbLP.process(context);
        reverbHP.process(context);
    }

    
    
    
//    TODO: get rid of all of the code except for the reverbConv.process(context) to see if there is an inefficiancy with the bypass/loading code

    float reverbAmount = *apvts.getRawParameterValue("REVERBAMOUNT");
    auto haasDelayTime = (*apvts.getRawParameterValue("HAASDELAYAMOUNT")/1000) * 44100;
    ampHeadSmoothedValues[5].setTargetValue(haasDelayTime);
    haasDelay.setDelay(ampHeadSmoothedValues[5].getNextValue());
    
    bool haasSide = *apvts.getRawParameterValue("HAASSIDE");
    bool haasBypass = *apvts.getRawParameterValue("HAASBYPASS");
    
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        auto* cleanSig = temp.getReadPointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            channelData[sample] = cleanSig[sample] * sqrt(1 - (reverbAmount/100)) + channelData[sample] * sqrt(reverbAmount/100);

            if (!haasBypass && ((haasSide && channel == 0) || (!haasSide && channel == 1))) {
                haasDelay.pushSample(0, channelData[sample]);
                channelData[sample] = haasDelay.popSample(0, -1, true);
            }
        }

    }

    auto outputGain =  juce::Decibels::decibelsToGain((float) *apvts.getRawParameterValue("OUTPUTGAIN"));
    ampHeadSmoothedValues[4].setTargetValue(outputGain);
    buffer.applyGain(ampHeadSmoothedValues[4].getNextValue());

//    //    Code to run post-amp levelmeter
    updateVolumeMeter(buffer, postAmpRmsLevelsSmoothed.data(), 2);
}

//==============================================================================
bool GuitarOscAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GuitarOscAudioProcessor::createEditor()
{
    return new GuitarOscAudioProcessorEditor (*this);
//    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void GuitarOscAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
//    std::unique_ptr<juce::XmlElement> xml(apvts.state.createXml());
//    copyXmlToBinary(*xml, destData);
    
    
    const auto state = apvts.copyState();
    const auto xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void GuitarOscAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    const auto xmlState = getXmlFromBinary(data, sizeInBytes);
    if (xmlState == nullptr)
        return;
    const auto newTree = juce::ValueTree::fromXml(*xmlState);
    apvts.replaceState(newTree);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuitarOscAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout GuitarOscAudioProcessor::createParameters()
{
    std::vector < std::unique_ptr < juce::RangedAudioParameter>> params;
    
//    AMP HEAD PARAMS
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"INPUTGAIN",1}, "input gain", juce::NormalisableRange<float>(-30.0f, 30.0f, 0.0001f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"BASSGAIN",2}, "bass gain", juce::NormalisableRange<float>(-20.0f, 20.0f, 0.01), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"MIDGAIN",3}, "mid gain", juce::NormalisableRange<float>(-10.0f, 10.0f, 0.01), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"TREBLEGAIN",3}, "treble gain", juce::NormalisableRange<float>(-10.0f, 10.0f, 0.01), 0.0f));
    
    juce::StringArray pages ("page 1", "page 2", "page 3");

    params.push_back(std::make_unique <juce::AudioParameterChoice>(juce::ParameterID {"PAGESELECTOR",4}, "page selector",pages,0));
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID {"CABYPASS",5}, "cab Bypass", false));
    
//    OVERDRIVE PARAMS
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"OVERDRIVEAMOUNT",6}, "treble gain", juce::NormalisableRange<float>(0.0f, 30.0f, 0.01), 10.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"OVERDRIVEBIAS",7}, "treble gain", juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"OVERDRIVETONE",8}, "treble gain", juce::NormalisableRange<float>(500.0f, 20000.0f, 0.01), 20000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"OVERDRIVELEVEL",9}, "treble gain", juce::NormalisableRange<float>(-30.0f, 0.0f, 0.01), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID {"OVERDRIVEBYPASS",10}, "Overdrive Bypass", false));
    
//    REVERB PARAMS
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID {"REVERBYPASS",11}, "Reverb Bypass", true));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"REVERBAMOUNT",12}, "Reverb Amount", juce::NormalisableRange<float>(0.0f, 100.0f, 0.01), 20.0f));
    
    juce::NormalisableRange<float> filterRange(10.0f, 20000.0f, 0.01);
    filterRange.setSkewForCentre(1000.0f);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"REVERBHIGHPASSFREQ",13}, "Reverb HP Freq", filterRange, 10.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"REVERBLOWPASSFREQ",14}, "Reverb LP Freq", filterRange, 20000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"HAASDELAYAMOUNT",15}, "Doubler Amount", juce::NormalisableRange<float>(0.0f,60.0f, 0.01),20.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID {"HAASBYPASS",16}, "Haas Bypass", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID {"HAASSIDE",17}, "Haas Side", true));

//    DELAY PARAMS
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"DELAYAMOUNT",18}, "Delay Amount", juce::NormalisableRange<float>(0.0f,100.0f, 0.01),30.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"DELAYFEEDBACK",19}, "Feedback Amount", juce::NormalisableRange<float>(0.0f,99.0f, 0.01),20.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"DELAYHIGHPASSFREQ",20}, "High pass freq",filterRange , 10.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"DELAYLOWPASSFREQ",21}, "Low pass freq", filterRange ,20000.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID {"DELAYBYPASS",22}, "Delay bypass", true));
    
    juce::NormalisableRange<float> delayTimeRange(0.0f,100000.0f, 0.01);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"DELAYTIME",24}, "Delay Time", delayTimeRange,1000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"OUTPUTGAIN",24}, "input gain", juce::NormalisableRange<float>(-30.0f, 30.0f, 0.0001f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID {"DELAYWIDTH",26}, "Delay Width", juce::NormalisableRange<float>(0.0f,80.0f, 0.01),70.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID {"PLUS15DB",27}, "plus 15 db", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID {"DELAYSNAP",28}, "Delay Snap", false));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID {"DELAYDIVISION",29}, "Delay Division",0,12,0));
    
//    REVERB SELECTOR PARAMS
    juce::StringArray reverbs;
    for (int i = 0; i <= 5; i++)
    {
        reverbs.add(BinaryData::namedResourceList[i]);
    }
    params.push_back(std::make_unique <juce::AudioParameterChoice>(juce::ParameterID {"REVERBSELECTOR",25}, "Reverb selector",reverbs,0));
    
    return { params.begin(), params.end() };
}


void GuitarOscAudioProcessor::updatePreEq()
{
    float peakFreq1 = 84;
    float peakGain1 = juce::Decibels::decibelsToGain(-0.1);
    float peakQ1 = 0.9;
    
//    1.1
    float peakFreq2 = 316;
    float peakGain2 = juce::Decibels::decibelsToGain(-9.8);
    float peakQ2 = 0.6;
    
    float peakFreq3 = 3900;
    float peakGain3 = juce::Decibels::decibelsToGain(6.4);
    float peakQ3 = 0.4;
    
    //5.7
    
    *parametric1.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), peakFreq1
        , peakQ1, peakGain1);
    
    *parametric2.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), peakFreq2
        , peakQ2, peakGain2);
    
    *parametric3.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), peakFreq3
        , peakQ3, peakGain3);
}

void GuitarOscAudioProcessor::updatePostEq()
{
//    Amp's post-eq curve
//    bass = below 100 hz
//    mid = 250-2khz
//    treble = above 2k
    
    float bassFreq = 115;
    auto bassGain = juce::Decibels::decibelsToGain((float) *apvts.getRawParameterValue("MIDGAIN"));
    ampHeadSmoothedValues[1].setTargetValue(bassGain);
    float BassQ = 1.30;
    
    float midFreq = 800;
    auto midGain = juce::Decibels::decibelsToGain((float) *apvts.getRawParameterValue("BASSGAIN"));
    ampHeadSmoothedValues[2].setTargetValue(midGain);
    float midQ = 0.46;
    
    float highFreq = 4000;
    auto highGain = juce::Decibels::decibelsToGain((float) *apvts.getRawParameterValue("TREBLEGAIN"));
    ampHeadSmoothedValues[3].setTargetValue(highGain);
    float highQ = 1.30;
    
    *bassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(getSampleRate(), bassFreq, BassQ,ampHeadSmoothedValues[2].getNextValue());
    *midFilter.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), midFreq, midQ,ampHeadSmoothedValues[1].getNextValue());
    *trebleFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(getSampleRate(), highFreq, highQ, ampHeadSmoothedValues[3].getNextValue());
}

void GuitarOscAudioProcessor::updateToneControl()
{
    float toneFreq = *apvts.getRawParameterValue("OVERDRIVETONE");
    toneFilterSmoothed.setTargetValue(toneFreq);
    *toneFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(),toneFilterSmoothed.getNextValue());
}
void GuitarOscAudioProcessor::updateReverbEq()
{
    float reverbHPF =  *apvts.getRawParameterValue("REVERBHIGHPASSFREQ");
    reverbFilterSmoothed[0].setTargetValue(reverbHPF);
    float reverbLPF =  *apvts.getRawParameterValue("REVERBLOWPASSFREQ");
    reverbFilterSmoothed[1].setTargetValue(reverbLPF);
    
    *reverbHP.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(),reverbFilterSmoothed[0].getNextValue());
    *reverbLP.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(),reverbFilterSmoothed[1].getNextValue());
}
void GuitarOscAudioProcessor::updateDelayEq()
{
    float delayHPF = *apvts.getRawParameterValue("DELAYHIGHPASSFREQ");
    delayFilterSmoothed[0].setTargetValue(delayHPF);
    float delayLPF =  *apvts.getRawParameterValue("DELAYLOWPASSFREQ");
    delayFilterSmoothed[1].setTargetValue(delayLPF);
    
    *delayHP.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(),delayFilterSmoothed[0].getNextValue());
    *delayLP.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(),delayFilterSmoothed[1].getNextValue());
    
}
void GuitarOscAudioProcessor::reset()
{
    parametric1.reset();
    parametric2.reset();
    parametric3.reset();
    bassFilter.reset();
    midFilter.reset();
    trebleFilter.reset();
    overDriveInputGain.reset();
    fxOverdrive.reset();
    overDriveBias.reset();
    toneFilter.reset();
    overDriveOutputGain.reset();
    reverbConv.reset();
    cabIrConv.reset();
    reverbHP.reset();
    reverbLP.reset();
    haasDelay.reset();
    delayHP.reset();
    delayLP.reset();
    delayWidthMixer.reset();
    delayMixer.reset();
    temp.clear();

    for (int i = 0; i < 6; i++)
    {
        ampHeadSmoothedValues[i].reset(getSampleRate(), 0.0007);
    }
    toneFilterSmoothed.reset(getSampleRate(), 0.0007);
    
    for (int i = 0; i < 2; i++)
    {
        reverbFilterSmoothed[i].reset(getSampleRate(), 0.0007);
        delayFilterSmoothed[i].reset(getSampleRate(), 0.0007);
    }
    
    delayTimeSmoothed.reset(getSampleRate(), 0.005);
    delayMixerSmoothed.reset(getSampleRate(), 0.0007);
}

void GuitarOscAudioProcessor::loadIR(){
    reverbConv.reset();
    reverbConv.loadImpulseResponse(irData, irSize,
    juce::dsp::Convolution::Stereo::yes,
    juce::dsp::Convolution::Trim::no, irSize, juce::dsp::Convolution::Normalise::yes);
}

void GuitarOscAudioProcessor::updateVolumeMeter(juce::AudioBuffer<float>& buffer, juce::SmoothedValue<float>* smoothedLevels, int numChannels)
{
    for (int i = 0; i < numChannels; i++) {
        smoothedLevels[i].skip(buffer.getNumSamples());
        float currentRmsLevel = juce::Decibels::gainToDecibels(buffer.getRMSLevel(i, 0, buffer.getNumSamples()));

        if (currentRmsLevel > smoothedLevels[i].getCurrentValue()) {
            smoothedLevels[i].setTargetValue(currentRmsLevel);
        } else {
            smoothedLevels[i].setCurrentAndTargetValue(currentRmsLevel);
        }
    }
}

double GuitarOscAudioProcessor::getBPM()
{
    return beatsPerMinute;
}
void GuitarOscAudioProcessor::setBPM(double bpm)
{
    beatsPerMinute = bpm;
}

double GuitarOscAudioProcessor::convertDelayToSnappedVal(int delVal)
{
    auto currValue = delVal;
    auto bpm = beatsPerMinute;

    if (bpm == 0)
    {
        return 0.0;
    }
    
    auto sampleRate = getSampleRate();
    const auto delMultiplier = 240.0;
    
    std::map<int, DelayInfo> delayInfoMap = {
        {12, { (delMultiplier * delayTimes["WholeNote"]) / bpm * sampleRate, "1/1" }},
        {11, { (delMultiplier * delayTimes["DottedHalfNote"]) / bpm * sampleRate, "3/2" }},
        {10, { (delMultiplier * delayTimes["HalfNote"]) / bpm * sampleRate, "1/2" }},
        {9, {  (delMultiplier * delayTimes["DottedQuarterNote"]) / bpm * sampleRate, "3/4" }},
        {8, {  (delMultiplier * delayTimes["TripletHalfNote"]) / bpm * sampleRate, "2/3" }},
        {7, {  (delMultiplier * delayTimes["QuarterNote"]) / bpm * sampleRate, "1/4" }},
        {6, {  (delMultiplier * delayTimes["DottedEighthNote"]) / bpm * sampleRate, "3/8" }},
        {5, {  (delMultiplier * delayTimes["TripletQuarterNote"]) / bpm * sampleRate, "2/3" }},
        {4, {  (delMultiplier * delayTimes["EighthNote"]) / bpm * sampleRate, "1/8" }},
        {3, {  (delMultiplier * delayTimes["DottedSixteenthNote"]) / bpm * sampleRate, "3/16" }},
        {2, {  (delMultiplier * delayTimes["TripletEighthNote"]) / bpm * sampleRate, "1/3" }},
        {1, {  (delMultiplier * delayTimes["SixteenthNote"]) / bpm * sampleRate, "1/16" }},
        {0, {  (delMultiplier * delayTimes["TripletSixteenthNote"]) / bpm * sampleRate, "1/24" }}
    };

    selected = delayInfoMap[currValue].delayName;
    
    if (delayInfoMap.count(currValue) > 0) {
        return delayInfoMap[currValue].sampleValue;
    }
    
    return 0.0;
}


float GuitarOscAudioProcessor::lerp(float* buffer, int size, float idx)
{
  auto i0 = int(idx);
  auto i1 = (i0 + 1) % size;
  auto x = idx - i0;
  return buffer[i0] + x * (buffer[i1] - buffer[i0]);
}
