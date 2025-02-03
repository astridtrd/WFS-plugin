/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PFE_WFS_simpleAudioProcessorEditor::PFE_WFS_simpleAudioProcessorEditor (PFE_WFS_simpleAudioProcessor& p, Parameters* params)
    : AudioProcessorEditor (&p), audioProcessor (p), params(params) 
{
    _speakerSourceDisplay._loudspeakerLayout = &p._loudspeakerLayout_config48L;    
    params->_pSpeakerSourceDisplay = &_speakerSourceDisplay;    
    params->_pSpeakerSourceDisplay->_speakerMask = params->_speakerMask;


    // create "Source" group and add Knobs to it
    _sourceGroup.setText("Source");
    _sourceGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    _sourceGroup.addAndMakeVisible(_sourcePosX_Knob);
    _sourceGroup.addAndMakeVisible(_sourcePosZ_Knob);
    _sourceGroup.addAndMakeVisible(_sourceRotation_Knob);
    addAndMakeVisible(_sourceGroup);

    // create "Listener" group and add Knobs to it
    _listenerGroup.setText("Listener");
    _listenerGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    _listenerGroup.addAndMakeVisible(_listenerPosX_Knob);
    _listenerGroup.addAndMakeVisible(_listenerPosZ_Knob);
    addAndMakeVisible(_listenerGroup);

    // create "SpeakerSourceDisplay" group and add the component to it
    _speakerSourceDisplayGroup.addAndMakeVisible(_speakerSourceDisplay);
    _speakerSourceDisplay.setBounds(0, 0, 450, 450);
    addAndMakeVisible(_speakerSourceDisplay);
    addAndMakeVisible(_speakerSourceDisplayGroup);

    // create "filtre" group and add Knobs to it
    _filterGroup.setText("filtre");
    _filterGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    _filterGroup.addAndMakeVisible(_bypassGain_Knob);
    _filterGroup.addAndMakeVisible(_bypassPH_Knob);
    addAndMakeVisible(_filterGroup);


    setSize (620, 700);


}

PFE_WFS_simpleAudioProcessorEditor::~PFE_WFS_simpleAudioProcessorEditor()
{
}

//==============================================================================
void PFE_WFS_simpleAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void PFE_WFS_simpleAudioProcessorEditor::resized()
{
    // set position of groups and childs
    _speakerSourceDisplayGroup.setBounds(10, 10, 480, 480);
    _speakerSourceDisplay.setTopLeftPosition(25, 30);
    _sourceGroup.setBounds(500, 10, 110, 250);
    _sourcePosX_Knob.setTopLeftPosition(20, 25);
    _sourcePosZ_Knob.setTopLeftPosition(20, 95);
    _sourceRotation_Knob.setTopLeftPosition(20, 165);
    _listenerGroup.setBounds(500, 260, 110, 180);
    _listenerPosX_Knob.setTopLeftPosition(20, 25);
    _listenerPosZ_Knob.setTopLeftPosition(20, 95);
    _filterGroup.setBounds(500, 510, 110, 200);
    _bypassGain_Knob.setTopLeftPosition(20, 25);
    _bypassPH_Knob.setTopLeftPosition(20, 95);
}
