/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
JucelawSelectablePannerAudioProcessor::JucelawSelectablePannerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  AudioChannelSet::stereo(), true)
#endif
                  .withOutput ("Output", AudioChannelSet::stereo(), true)
#endif
                  )
#endif
{
    addParameter(panValue = new AudioParameterFloat("panValue",
                                                    "PanValue",
                                                    NormalisableRange<float>(0.0f, 1.0f),
                                                    0.5f));
    smooth.setValue(0.5f);
}

JucelawSelectablePannerAudioProcessor::~JucelawSelectablePannerAudioProcessor()
{
}

//==============================================================================
const String JucelawSelectablePannerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JucelawSelectablePannerAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool JucelawSelectablePannerAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

double JucelawSelectablePannerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JucelawSelectablePannerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int JucelawSelectablePannerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JucelawSelectablePannerAudioProcessor::setCurrentProgram (int index)
{
}

const String JucelawSelectablePannerAudioProcessor::getProgramName (int index)
{
    return String();
}

void JucelawSelectablePannerAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void JucelawSelectablePannerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    smooth.reset(sampleRate, smoothingLengthInSeconds);
    //    setScaledPanValue();
}

void JucelawSelectablePannerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JucelawSelectablePannerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;
    
    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif
    
    return true;
#endif
}
#endif

void JucelawSelectablePannerAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int totalNumInputChannels = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    float** channelDataArray = buffer.getArrayOfWritePointers();
    const int bufSize= buffer.getNumSamples();
    double Lgain;
    double Rgain;
    
    if ((double)*panValue != lastPanValue)
    {
        smooth.setValue(std::min(std::max(0.0, (double)*panValue), 1.0));
    }
    
    for (int sample = 0; sample < bufSize; ++sample)
    {
        double currentSmoothedPanValue = smooth.getNextValue();
        if (currentSmoothedPanValue < 0.0 || currentSmoothedPanValue > 1.0)
        {
            //LinearSmoothedValueでパンの動きを線形補間した際に、値が0.0~1.0の範囲外に行ってしまうことがある
            //範囲外に行った時は直近の値を保持し、LinearSmoothedValue.setValue()に0.0か1.0を入れる。
            currentSmoothedPanValue = lastSmoothedPanValue;
            if (currentSmoothedPanValue < 0.0)
                smooth.setValue(0.0);
            if (currentSmoothedPanValue > 1.0)
                smooth.setValue(1.0);
        }
        jassert(0.0 <= currentSmoothedPanValue && currentSmoothedPanValue <= 1.0);
        switch (panAlgoList)
        {
            case panningAlgorithm::ConstantPower://均等パワー
            {
                double scaledCurrentPan = currentSmoothedPanValue * (double_Pi / 2.0) - double_Pi / 4.0;
                Lgain = (cos(scaledCurrentPan) - sin(scaledCurrentPan)) * sqrt(2.0) / 2.0;
                Rgain = (cos(scaledCurrentPan) + sin(scaledCurrentPan)) * sqrt(2.0) / 2.0;
                for (int channel = 0; channel < totalNumInputChannels; ++channel)
                {
                    if (channel % 2 == 0)
                    {
                        //L channel
                        channelDataArray[channel][sample] *= Lgain;
                    }
                    else
                    {
                        //R channel
                        channelDataArray[channel][sample] *= Rgain;
                    }
                }
                break;
            }
            case panningAlgorithm::ConstantGain://均等ゲイン
            {
                Lgain = 1.0 - currentSmoothedPanValue;
                Rgain = currentSmoothedPanValue;
                for (int channel = 0; channel < totalNumInputChannels; ++channel)
                {
                    if (channel % 2 == 0)
                    {
                        //L channel
                        channelDataArray[channel][sample] *= Lgain;
                    }
                    else
                    {
                        //R channel
                        channelDataArray[channel][sample] *= Rgain;
                    }
                }
                break;
            }
            case panningAlgorithm::Log3://Log(-3dB)
            {
                double logValue_amp = Decibels::decibelsToGain(-3.0);
                Lgain = pow(1.0 - currentSmoothedPanValue, log(logValue_amp)/log(0.5));
                Rgain = pow(currentSmoothedPanValue, log(logValue_amp)/log(0.5));
                for (int channel = 0; channel < totalNumInputChannels; ++channel)
                {
                    if (channel % 2 == 0)
                    {
                        //L channel
                        channelDataArray[channel][sample] *= Lgain;
                    }
                    else
                    {
                        //R channel
                        channelDataArray[channel][sample] *= Rgain;
                    }
                }
                break;
            }
            case panningAlgorithm::Log4_5://Log(-4.5dB)
            {
                double logValue_amp = Decibels::decibelsToGain(-4.5);
                Lgain = pow(1.0 - currentSmoothedPanValue, log(logValue_amp)/log(0.5));
                Rgain = pow(currentSmoothedPanValue, log(logValue_amp)/log(0.5));
                for (int channel = 0; channel < totalNumInputChannels; ++channel)
                {
                    if (channel % 2 == 0)
                    {
                        //L channel
                        channelDataArray[channel][sample] *= Lgain;
                    }
                    else
                    {
                        //R channel
                        channelDataArray[channel][sample] *= Rgain;
                    }
                }
                break;
            }
            case panningAlgorithm::Log6://Log(-6dB)
            {
                double logValue_amp = Decibels::decibelsToGain(-6.0);
                Lgain = pow(1.0 - currentSmoothedPanValue, log(logValue_amp)/log(0.5));
                Rgain = pow(currentSmoothedPanValue, log(logValue_amp)/log(0.5));
                for (int channel = 0; channel < totalNumInputChannels; ++channel)
                {
                    if (channel % 2 == 0)
                    {
                        //L channel
                        channelDataArray[channel][sample] *= Lgain;
                    }
                    else
                    {
                        //R channel
                        channelDataArray[channel][sample] *= Rgain;
                    }
                }
                break;
            }
            default:
            {
                jassertfalse;
                break;
            }
        }
        lastSmoothedPanValue = currentSmoothedPanValue;
    }
    lastPanValue = (double)*panValue;
}

//==============================================================================
bool JucelawSelectablePannerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* JucelawSelectablePannerAudioProcessor::createEditor()
{
    return new JucelawSelectablePannerAudioProcessorEditor (*this);
}

//==============================================================================
void JucelawSelectablePannerAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    ScopedPointer<XmlElement> xml(new XmlElement("paramXml"));
    xml->setAttribute(panValue->name, (double)*panValue);
    xml->setAttribute("panAlgoList", (int)static_cast<int>(panAlgoList));
    copyXmlToBinary(*xml, destData);
}

void JucelawSelectablePannerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    ScopedPointer<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr)
    {
        if (xmlState->hasTagName("paramXml"))
        {
            *panValue = (float)xmlState->getDoubleAttribute(panValue->name, 0.5f);
            panAlgoList = static_cast<panningAlgorithm>((int)xmlState->getIntAttribute("panAlgoList"));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JucelawSelectablePannerAudioProcessor();
}
