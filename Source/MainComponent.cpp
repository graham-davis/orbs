/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "./Sine.h"
#include "./Smooth.h"
#include "./Sequencer.h"
#include "./Node.h"
#include "./FaustReverb.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent :
    public AudioAppComponent,
    private Slider::Listener,
    private Timer
{
public:
    //==============================================================================
    MainContentComponent(): onOff(1.0),gain(0.5)
    {
        addAndMakeVisible(tempoSlider);
        tempoSlider.setRange(100.0, 400.0);
        tempoSlider.setValue(100.0);
        tempoSlider.setTextBoxIsEditable(false);
        tempoSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
        tempoSlider.addListener(this);
        
        addAndMakeVisible(modulationSlider);
        modulationSlider.setRange(30.0, 200.0);
        modulationSlider.setValue(30.0);
        modulationSlider.setTextBoxIsEditable(false);
        modulationSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
        modulationSlider.addListener(this);
        
        addAndMakeVisible(gainSlider);
        gainSlider.setRange(0.0, 1.0);
        gainSlider.setValue(0.5);
        gainSlider.setTextBoxIsEditable(false);
        gainSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
        gainSlider.addListener(this);
        
        addAndMakeVisible(indexSlider);
        indexSlider.setRange(10.0, 400.0);
        indexSlider.setValue(10.0);
        indexSlider.setTextBoxIsEditable(false);
        indexSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
        indexSlider.addListener(this);
        
        
        setSize (750, 400);

        // specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
        audioBuffer = new float*[2];
    }

    ~MainContentComponent()
    {
        shutdownAudio();
        delete [] audioBuffer;
    }

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        rate = sampleRate;
        
        // Initialize sequencer parameters
        sequencer.setSamplingRate(sampleRate);
        sequencer.setTempo(tempoSlider.getValue());
        sequencer.setModulator(modulationSlider.getValue());
        sequencer.prepareToPlay();
        
        // Initialize Faust reverb
        reverb.init(sampleRate);
        reverb.buildUserInterface(&reverbControl);
        
        for(int i=0; i<2; i++) {
            smooth[i].setSmooth(.999);
        }
        
        startTimer(60000/tempoSlider.getValue());
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        for(int i=0; i<2; i++) {
            audioBuffer[i] = bufferToFill.buffer->getWritePointer (i, bufferToFill.startSample);
        }
        
        // computing one block
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            double newSample = sequencer.tick()*smooth[0].tick(onOff)*smooth[1].tick(gain);
            audioBuffer[0][sample] = newSample;
        }
        reverb.compute(bufferToFill.numSamples, audioBuffer, audioBuffer);
    }

    void releaseResources() override
    {
        // This will be called when the audio device stops, or when it is being
        // restarted due to a setting change.

        // For more details, see the help for AudioProcessor::releaseResources()
    }

    //==============================================================================
    void paint (Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (Colour::fromRGB(40, 40 ,40));
        
        addAndMakeVisible(sequencer);
        
        for (int i=0; i<8; i++) {
            addAndMakeVisible(node[i]);
            node[i].setNodeNumber(i);
            node[i].setSequencer(&sequencer);
        }
        
        g.setColour(Colour::fromRGB(193, 199, 218));
        g.drawLine(10, 95, getWidth()-10, 95, 3);
    }

    void resized() override
    {
        Rectangle<int> area = getLocalBounds();
        {
            sequencer.setBounds(area.removeFromBottom(10));
            
            Rectangle<int> nodeArea = area.removeFromBottom(300);
            for(int i=0; i<8; i++) {
                node[i].setBounds(nodeArea.removeFromLeft(getWidth()/8));
            }
            
            Rectangle<int> controlArea = area.removeFromTop(100);
            Rectangle<int> topControlArea = controlArea.removeFromTop(50);
            tempoSlider.setBounds(topControlArea.removeFromLeft(getWidth()/2));
            modulationSlider.setBounds(topControlArea.removeFromRight(getWidth()/2));
            Rectangle<int> bottomControlArea = controlArea.removeFromBottom(50);
            gainSlider.setBounds(bottomControlArea.removeFromLeft(getWidth()/2));
            indexSlider.setBounds(bottomControlArea.removeFromRight(getWidth()/2));

        }
    }
    
    void sliderValueChanged (Slider* slider) override
    {
        if (slider == &tempoSlider){
            sequencer.setTempo(slider->getValue());
            stopTimer();
            startTimer(60000/tempoSlider.getValue());
        } else if (slider == &modulationSlider){
            sequencer.setModulator(slider->getValue());
        } else if (slider == &indexSlider) {
            sequencer.setIndex(slider->getValue());
        } else if (slider == &gainSlider) {
            gain = slider->getValue();
        }
    }
    
    void timerCallback() override
    {
        for(int i=0; i<8; i++) {
            node[i].checkNote();
        }
    }


private:
    Sequencer sequencer;
    Node node[8];
    Smooth smooth[2];
    
    Slider tempoSlider, gainSlider, modulationSlider, indexSlider;
    
    FaustReverb reverb;
    MapUI reverbControl;
    
    int onOff;
    double rate;
    int tempo;
    float gain;
    
    float** audioBuffer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

Component* createMainContentComponent()     { return new MainContentComponent(); }

#endif  // MAINCOMPONENT_H_INCLUDED
