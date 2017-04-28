/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
JucelawSelectablePannerAudioProcessorEditor::JucelawSelectablePannerAudioProcessorEditor (JucelawSelectablePannerAudioProcessor& p)
: AudioProcessorEditor (&p), processor (p)
{
    //Panning Law選択用ComboBox
    addAndMakeVisible (panAlgoCombo = new ComboBox ("panAlgoComboBox"));
    panAlgoCombo->setEditableText (false);
    panAlgoCombo->setColour(ComboBox::textColourId, Colours::black);
    panAlgoCombo->setColour(ComboBox::arrowColourId, Colours::black);
    panAlgoCombo->setColour(ComboBox::backgroundColourId, Colours::white);
    panAlgoCombo->setJustificationType (Justification::centred);
    panAlgoCombo->setTextWhenNothingSelected (String());
    panAlgoCombo->setTextWhenNoChoicesAvailable (String());
    panAlgoCombo->addItem (TRANS("Constant Power"), 1);
    panAlgoCombo->addItem (TRANS("Constant Gain"), 2);
    panAlgoCombo->addItem(TRANS("Log -3dB"), 3);
    panAlgoCombo->addItem(TRANS("Log -4.5dB"), 4);
    panAlgoCombo->addItem(TRANS("Log -6dB"), 5);
    panAlgoCombo->setSelectedItemIndex(static_cast<int>(processor.panAlgoList), NotificationType::dontSendNotification);
    panAlgoCombo->addListener (this);
    
    //Pan調整用Slider
    addAndMakeVisible(panPod = new Slider(processor.panValue->name));
    panPod->setRange(processor.panValue->range.start, processor.panValue->range.end);
    panPod->setSliderStyle(Slider::RotaryVerticalDrag);
    panPod->setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    panPod->setValue(*processor.panValue, NotificationType::dontSendNotification);
    panPod->setDoubleClickReturnValue(true, 0.5f);
    panPod->addListener(this);
    
    //PanningLaw計算式の表示用Label
    addAndMakeVisible(panAlgoDescriptionLabel = new Label);
    panAlgoDescriptionLabel->setColour(Label::textColourId, Colours::black);
    panAlgoDescriptionLabel->setColour(Label::backgroundColourId, Colours::white);
    panAlgoDescriptionLabel->setFont(Font(11.0));
    panAlgoDescriptionLabel->setJustificationType(Justification::topLeft);
    panAlgoDescriptionLabel->setText("-pi/4 <= Pan <= pi/4\nLch:(cos(Pan) - sin(Pan)) * sqrt(2) / 2\nRch:(cos(Pan) + sin(Pan)) * sqrt(2) / 2", NotificationType::dontSendNotification);
    
    //署名用Label
    addAndMakeVisible(signatureLabel = new Label);
    signatureLabel->setColour(Label::textColourId, Colours::black);
    signatureLabel->setColour(Label::backgroundColourId, Colours::white);
    signatureLabel->setFont(Font(11.0));
    signatureLabel->setJustificationType(Justification::bottomRight);
    signatureLabel->setText("Akiyuki Okayasu", NotificationType::dontSendNotification);
    
    //アイコン画像
    icon = ImageCache::getFromMemory(BinaryData::Icon_AkiyukiOkayasu_png, BinaryData::Icon_AkiyukiOkayasu_pngSize);
    
    setSize(300, 300);
    startTimerHz(60);
}

JucelawSelectablePannerAudioProcessorEditor::~JucelawSelectablePannerAudioProcessorEditor()
{
}

//==============================================================================
void JucelawSelectablePannerAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colour::fromRGB(28, 35, 45));
    g.drawImageWithin(icon, getWidth() - icon.getWidth(), getHeight() - icon.getHeight(), icon.getWidth(), icon.getHeight(), RectanglePlacement::yTop, false);
}

void JucelawSelectablePannerAudioProcessorEditor::resized()
{
    auto r = getLocalBounds();
    auto signatureBounds = r.removeFromBottom(100);
    auto iconBounds = signatureBounds.removeFromRight(100);
    auto panAlgoDescriptionBounds = signatureBounds.removeFromTop(85);
    auto panAlgoBounds = r.removeFromTop(25);
    panPod->setBounds(r.reduced(10));
    panAlgoCombo->setBounds(panAlgoBounds);
    panAlgoDescriptionLabel->setBounds(panAlgoDescriptionBounds);
    signatureLabel->setBounds(signatureBounds);
    
}

void JucelawSelectablePannerAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    if (slider == panPod)
    {
        if(AudioProcessorParameter* param = static_cast<AudioProcessorParameter*>(processor.panValue))
        {
            if (slider->isMouseButtonDown())
            {
                param->setValueNotifyingHost((float)slider->getValue());
            }
            else
            {
                param->setValue((float)slider->getValue());
            }
        }
    }
}

void JucelawSelectablePannerAudioProcessorEditor::sliderDragStarted (Slider* slider)
{
    if (slider == panPod)
    {
        if (AudioProcessorParameter* param = static_cast<AudioProcessorParameter*>(processor.panValue))
        {
            param->beginChangeGesture();
        }
    }
}

void JucelawSelectablePannerAudioProcessorEditor::sliderDragEnded (Slider* slider)
{
    if (slider == panPod)
    {
        if (AudioProcessorParameter* param = static_cast<AudioProcessorParameter*>(processor.panValue))
        {
            param->endChangeGesture();
        }
    }
}

void JucelawSelectablePannerAudioProcessorEditor::comboBoxChanged (ComboBox* combo)
{
    if (combo == panAlgoCombo)
    {
        const int algoIndex = combo->getSelectedItemIndex();
        processor.panAlgoList = static_cast<panningAlgorithm>(algoIndex);
        switch (processor.panAlgoList) {
            case panningAlgorithm::ConstantPower://均等パワー
                panAlgoDescriptionLabel->setText("-pi/4 <= pan <= pi/4\nLch : (cos(pan) - sin(pan)) * sqrt(2) / 2\nRch : (cos(pan) + sin(pan)) * sqrt(2) / 2", NotificationType::dontSendNotification);
                break;
            case panningAlgorithm::ConstantGain://均等ゲイン
                panAlgoDescriptionLabel->setText("0 <= pan <= 1\nLch : (1 - pan)\nRch : pan", NotificationType::dontSendNotification);
                break;
            case panningAlgorithm::Log3://Log(-3dB)
                panAlgoDescriptionLabel->setText("0 <= pan <= 1\nvalue = Decibels::decibelsToGain(-3)\nLch : pow(1 - pan, log(value) / log(0.5))\nRch : pow(pan, log(value) / log(0.5))", NotificationType::dontSendNotification);
                break;
            case panningAlgorithm::Log4_5://Log(-4.5dB)
                panAlgoDescriptionLabel->setText("0 <= pan <= 1\nvalue = Decibels::decibelsToGain(-4.5)\nLch : pow(1 - pan, log(value) / log(0.5))\nRch : pow(pan, log(value) / log(0.5))", NotificationType::dontSendNotification);
                break;
            case panningAlgorithm::Log6://Log(-6dB)
                panAlgoDescriptionLabel->setText("0 <= pan <= 1\nvalue = Decibels::decibelsToGain(-6)\nLch : pow(1 - pan, log(value) / log(0.5))\nRch : pow(pan, log(value) / log(0.5))", NotificationType::dontSendNotification);
                break;
            default:
                jassertfalse;
                panAlgoDescriptionLabel->setText("invalid pan algo", NotificationType::dontSendNotification);
                break;
        }
    }
}

void JucelawSelectablePannerAudioProcessorEditor::timerCallback()
{
    panPod->setValue(*processor.panValue, NotificationType::dontSendNotification);
}
