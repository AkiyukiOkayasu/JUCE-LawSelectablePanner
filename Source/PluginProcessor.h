/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

enum class panningAlgorithm : int
{
    ConstantPower,
    ConstantGain,
    Log3,
    Log4_5,
    Log6,
    NumPanningAlgorithm
};

//==============================================================================
/**
 */
class JucelawSelectablePannerAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    JucelawSelectablePannerAudioProcessor();
    ~JucelawSelectablePannerAudioProcessor();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    
#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
    
    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;
    
    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    //==============================================================================
    const String getName() const override;
    
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;
    
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;
    
    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    AudioParameterFloat* panValue;
    panningAlgorithm panAlgoList = panningAlgorithm::ConstantPower;
    LinearSmoothedValue<double> smooth;
private:
    const double smoothingLengthInSeconds = 0.01;//パン変更時にクリックノイズを鳴らさないためにパンの値に10msの線形補間をかける
    double lastPanValue = -1.0;
    double lastSmoothedPanValue = 0.5;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JucelawSelectablePannerAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
