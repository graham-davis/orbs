/*
  ==============================================================================

    Sequencer.h
    Created: 10 Oct 2016 7:09:47pm
    Author:  Graham Davis

  ==============================================================================
*/

#ifndef SEQUENCER_H_INCLUDED
#define SEQUENCER_H_INCLUDED

#include "./Sine.h"
#include "./Smooth.h"

class Sequencer : public Component{
public:
    Sequencer():currentSampleRate(0.0),samplesPerBeat(0.0),currentSample(0.0),currentNote(0.0),currentIndex(10){
        for(int i=0; i<4; i++){
            smooth[i].setSmooth(0.997);
        }
        frequencies = new int[8];
    }
    
    ~Sequencer(){
        delete [] frequencies;
    }
    
    void paint (Graphics& g) override
    {
    }
    
    void resized() override
    {
    }
    
    // Sampling rate must be set to get an accurate frequency
    void setSamplingRate(int samplingRate){
        currentSampleRate = samplingRate;
        carrier.setSamplingRate(samplingRate);
        modulator.setSamplingRate(samplingRate);
    }
    
    // Sets the tempo of the sequencer
    void setTempo(int tempo){
        float beatsPerSecond = tempo / 60.0;
        samplesPerBeat =  currentSampleRate  / beatsPerSecond;
    }
    
    // Sets modulator frequency
    void setModulator(float frequency) {
        modulator.setFrequency(frequency);
    }
    
    // Sets modulator index
    void setIndex(float index) {
        currentIndex = index;
    }
    
    // Initialize necessary parts to play
    void prepareToPlay() {
        for(int i=0; i<8; i++) {
            noteOn[i] = 0;
            frequencies[i] = 100;
        }
    }
    
    // Turn on/off specific note
    void toggleNote(int note) {
        noteOn[note] = !noteOn[note];
    }
    
    // Set a node's frequency
    void setNodeFrequency(int node, int frequency) {
        frequencies[node] = frequency;
    }
    
    int getCurrentNode() {
        return currentNote;
    }

    // compute one sample
    double tick(){
        if (currentSample > samplesPerBeat) {
            currentSample = 0;
            updateFrequency();
        } else {
            currentSample++;
        }
        carrier.setFrequency(smooth[0].tick(frequencies[currentNote]) + modulator.tick()*currentIndex);
        return carrier.tick() * (smooth[1].tick(noteOn[currentNote]));
    }
    
    // This function updates the current note of the sequencer
    
    void updateFrequency() {
        if (currentNote < 7) {
            currentNote++;
        } else {
            currentNote = 0;
        }
    }
    
private:
    int currentSampleRate;
    int samplesPerBeat;
    int currentSample;
    int currentNote;
    int noteOn[8];
    int* frequencies;
    
    float currentIndex;
    
    Smooth smooth[4];
    
    Sine carrier, modulator;
};


#endif  // SEQUENCER_H_INCLUDED
