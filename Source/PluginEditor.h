/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
 */
class JucelawSelectablePannerAudioProcessorEditor  :
public AudioProcessorEditor,
public ComboBox::Listener,
public Slider::Listener,
private Timer
{
public:
    JucelawSelectablePannerAudioProcessorEditor (JucelawSelectablePannerAudioProcessor&);
    ~JucelawSelectablePannerAudioProcessorEditor();
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void sliderValueChanged (Slider* slider) override;
    void sliderDragStarted (Slider* slider) override;
    void sliderDragEnded (Slider* slider) override;
    void comboBoxChanged (ComboBox* combo) override;
    
private:
    void timerCallback() override;
    
    JucelawSelectablePannerAudioProcessor& processor;
    
    ScopedPointer<Slider> panPod;
    ScopedPointer<ComboBox> panAlgoCombo;
    ScopedPointer<Label> panAlgoLabel;
    ScopedPointer<Label> panAlgoDescriptionLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JucelawSelectablePannerAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
