/*
  ==============================================================================

    PresetManager.cpp
    Created: 5 Feb 2023 6:29:21pm
    Author:  Noah Deetz

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PresetManager.h"

//==============================================================================
const juce::File PresetManager::defaultDirectory {juce::File::getSpecialLocation(juce::File::SpecialLocationType::commonDocumentsDirectory).getChildFile(ProjectInfo::companyName)
    .getChildFile(ProjectInfo::projectName)
};
const juce::String PresetManager::extension { "preset"};
const juce::String PresetManager::presetNameProperty {"presetName"};

PresetManager::PresetManager(juce::AudioProcessorValueTreeState& apvts) :
        valueTreeState(apvts)
{
//    create default preset dir if it is not there
    if (defaultDirectory.exists() == false)
    {
        const auto result = defaultDirectory.createDirectory();
        if (result.failed())
        {
            DBG("could not create preset directory: " + result.getErrorMessage());
            jassertfalse;
        }
    }
    
    valueTreeState.state.addListener(this);
    currentPreset.referTo(valueTreeState.state.getPropertyAsValue(presetNameProperty, nullptr));
    
}


void PresetManager::savePreset(const juce::String &presetName)
{
   if(presetName.isEmpty())
   {
       return;
   }
    currentPreset.setValue(presetName);
    const auto xml = valueTreeState.copyState().createXml();
    const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
    if (xml->writeTo(presetFile) == false)
    {
        DBG("could not create preset file" + presetFile.getFullPathName());
        jassertfalse;
    }
    
}
void PresetManager::deletePreset(const juce::String &presetName)
{
    if (comboId < numElements)
    {
        auto xml = juce::parseXML (juce::CharPointer_UTF8 (factoryPresets[comboId]));
        auto stringName = xml->getStringAttribute("presetName");
       
        if (presetName == stringName)
        {
            return;
        }
    }
//    make it not crahs
    
    if (presetName.isEmpty())
    {
        return;
    }
    const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
    if (comboId != -1 && presetFile.existsAsFile() == false)
    {
        DBG("Preset file cannot be deleted becuase it is a factory preset. ");
        jassertfalse;
        return;
    }
    if (presetFile.deleteFile() == false)
    {
        DBG("Preset File: " + presetFile.getFullPathName() + " could not be deleted");
        jassertfalse;
        return;
    }
    currentPreset.setValue("");
    
}
void PresetManager::loadPreset(const juce::String &presetName)
{
    if (presetName.isEmpty())
    {
        return;
    }
    const auto& presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
    if (presetFile.existsAsFile() == false)
    {
            auto currentIndex = getComboId();
            if (currentIndex < numElements)
            {
                auto xml = juce::XmlDocument(factoryPresets[currentIndex]);
                const auto valueTreeToLoad = juce::ValueTree::fromXml(*xml.getDocumentElement());
                
                valueTreeState.replaceState(valueTreeToLoad);
                currentPreset.setValue(presetName);
            }
    } else
    {
        juce::XmlDocument xmlDocument {presetFile};

        const auto valueTreeToLoad = juce::ValueTree::fromXml(*xmlDocument.getDocumentElement());
        valueTreeState.replaceState(valueTreeToLoad);
        currentPreset.setValue(presetName);
    }
}
int PresetManager::loadNextPreset()
{
    const auto& allPresets = getAllPresets();
    if (allPresets.isEmpty())
    {
        return -1;
    }
    const auto currentIndex = allPresets.indexOf(currentPreset.toString());
    const auto nextIndex = currentIndex + 1 > (allPresets.size() - 1) ? 0 : currentIndex + 1;
    
    loadPreset(allPresets.getReference(nextIndex));

    return nextIndex;
    
}
int PresetManager::loadPreviousPreset()
{
    const auto& allPresets = getAllPresets();
    if (allPresets.isEmpty())
    {
        return -1;
    }
    const auto currentIndex = allPresets.indexOf(currentPreset.toString());
    const auto previousIndex = currentIndex - 1 < 0 ? allPresets.size() - 1 : currentIndex - 1;
    loadPreset(allPresets.getReference(previousIndex));
    return previousIndex;
    
}
juce::StringArray PresetManager::getAllPresets() const
{
    juce::StringArray presets;
    
    const auto fileArray = defaultDirectory.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*." + extension);
    
    for (int i = 0; i < numElements; i++)
    {
        auto xml = juce::parseXML (juce::CharPointer_UTF8 (factoryPresets[i]));
        auto stringName = xml->getStringAttribute("presetName");
        presets.add(stringName);
    }
    
    for (const auto& file : fileArray)
    {
        presets.add(file.getFileNameWithoutExtension());
    }
    
    return presets;
}
juce::String PresetManager::getCurrentPreset() const
{
    return currentPreset.toString();
}
void PresetManager::valueTreeRedirected(juce::ValueTree &changedTree)
{
    currentPreset.referTo(changedTree.getPropertyAsValue(presetNameProperty, nullptr));
}
void PresetManager::setComboId(int idx)
{
    comboId = idx;
}
int PresetManager::getComboId()
{
    return comboId;
}
