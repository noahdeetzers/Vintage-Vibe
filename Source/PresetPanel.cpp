/*
  ==============================================================================

    PresetPanel.cpp
    Created: 5 Feb 2023 6:00:47pm
    Author:  Noah Deetz

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PresetPanel.h"

//==============================================================================
PresetPanel::PresetPanel( PresetManager& pm) : presetManager(pm)
{
    configureButton(saveButton, "save");
    configureButton(deleteButton, "delete");
    configureButton(previousPresetButton, "<");
    configureButton(nextPresetButton, ">");


    presetList.setTextWhenNothingSelected("no preset selected");
    presetList.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    presetList.setColour(juce::ComboBox::backgroundColourId, juce::Colours::white.withBrightness(0.25f));
    presetList.setColour(juce::ComboBox::focusedOutlineColourId, juce::Colours::white.withBrightness(0.25f));
    addAndMakeVisible(presetList);
    presetList.addListener(this);
    presetList.setSelectedItemIndex(-1);
    loadPresetList();
}


PresetPanel::~PresetPanel()
{
    saveButton.removeListener(this);
    deleteButton.removeListener(this);
    previousPresetButton.removeListener(this);
    nextPresetButton.removeListener(this);
    
    presetList.removeListener(this);
}

void PresetPanel::paint (juce::Graphics& g)
{
//        DBG(presetList.getSelectedItemIndex());

}

void PresetPanel::resized()
{
    const auto container = getLocalBounds().reduced(4);
    auto bounds = container;
    
    saveButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.2f)).reduced(4));
    previousPresetButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(4));
    presetList.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.4f)).reduced(4));
    nextPresetButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(4));
    deleteButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.2f)).reduced(4));
    
}

void PresetPanel::buttonClicked(juce::Button *button)
{
    if (button == &saveButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Please enter the name of the preset to save",
            PresetManager::defaultDirectory,
            "*." + PresetManager::extension
        );
        fileChooser->launchAsync(juce::FileBrowserComponent::saveMode, [&](const juce::FileChooser& chooser){
            const auto resultFile = chooser.getResult();
            presetManager.savePreset(resultFile.getFileNameWithoutExtension());
            loadPresetList();
        });
    }
    if (button == &previousPresetButton)
    {
        decrement();
    }
    if (button == &nextPresetButton)
    {
        increment();
    }
    if (button == &deleteButton)
    {
        
        const auto callback = juce::ModalCallbackFunction::create ([this](int result) {
            if (result == 0) {}// result == 0 means you click Cancel
            if (result == 1){
                presetManager.deletePreset(presetManager.getCurrentPreset());
                loadPresetList();
            }// result == 1 means you click OK
        });
        
        deletionWarning.showOkCancelBox(juce::MessageBoxIconType::WarningIcon, "Deleting Preset", "Are you sure you want to delete this preset?", "YES", "CANCEL", nullptr, callback);

        
//        presetManager.deletePreset(presetManager.getCurrentPreset());
//        loadPresetList();
    }
}

void PresetPanel::configureButton(juce::Button &button, const juce::String &text)
{
    button.setButtonText(text);
    button.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    button.setColour(juce::TextButton::buttonColourId, juce::Colours::white.withBrightness(0.25f));
    addAndMakeVisible(button);
    button.addListener(this);
}

void PresetPanel::comboBoxChanged(juce::ComboBox *combobox)
{
    if (combobox == &presetList)
    {
        presetManager.setComboId(presetList.getSelectedItemIndex());
        presetManager.loadPreset(presetList.getItemText(presetList.getSelectedItemIndex()));
    }
}

void PresetPanel::loadPresetList()
{
    
    presetList.clear(juce::dontSendNotification);
    const auto allPresets = presetManager.getAllPresets();
    const auto currentPreset = presetManager.getCurrentPreset();
    presetList.addItemList(presetManager.getAllPresets(), 1);
    presetList.setSelectedId(allPresets.indexOf(currentPreset) + 1, juce::dontSendNotification);
}

void PresetPanel::increment()
{
    presetList.setSelectedItemIndex(presetList.getSelectedItemIndex() + 1);
}
void PresetPanel::decrement()
{
    presetList.setSelectedItemIndex(presetList.getSelectedItemIndex() - 1);
}
