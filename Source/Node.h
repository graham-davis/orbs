/*
  ==============================================================================

    Node.h
    Created: 10 Oct 2016 8:28:03pm
    Author:  Graham Davis

  ==============================================================================
*/

#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include "./Sequencer.h"


class Node    : public Component{
public:
    Node():nodeNumber(0.0),onOff(0.0),currentNode(0.0),newY(210),yPos(210){}
    
    ~Node(){}
    
    void paint (Graphics& g) override
    {
        Rectangle<float> area (getLocalBounds().toFloat().reduced (2.0f));
        
        if(dragging) {
            newY = yPos+dragY;
            if (newY < 5) newY = 5;
            if (newY > 210) newY = 210;
            currentSequencer->setNodeFrequency(nodeNumber, 1250-newY*5);
        }
        
        g.setColour (Colour::fromRGB(193, 199, 218));
        if (currentNode) {
            g.fillRoundedRectangle(getWidth()/2-5, newY+30, 10, getHeight()-newY, 10);
        } else {
            g.drawRoundedRectangle(getWidth()/2-5, newY+30, 10, getHeight()-newY, 10, 2);
        }

        g.drawEllipse (getWidth()/2-30, newY, 60, 60, 3);
        
        if(onOff==1) {
            g.setColour(Colour::fromRGB(117, 129, 168));
            g.fillEllipse(getWidth()/2-30, newY, 60, 60);
        } else {
            g.setColour(Colours::darkgrey);
            g.fillEllipse(getWidth()/2-30, newY, 60, 60);
        }
    }

    void resized() override
    {
        // Just set the limits of our constrainer so that we don't drag ourselves off the screen
        constrainer.setMinimumOnscreenAmounts (getHeight(), getWidth(), getHeight(), getWidth());
    }

    void mouseDoubleClick (const MouseEvent& e) override
    {
        onOff = !onOff;
        currentSequencer->toggleNote(nodeNumber);
        repaint();
    }
    
    void mouseDrag (const MouseEvent& e) override
    {
        dragging = 1;
        dragY = e.getDistanceFromDragStartY();
        repaint();
    }
        
    void mouseUp (const MouseEvent& e) override
    {
        if (dragging) {
            dragging = 0;
            yPos = newY;
        }
    }

    // Sets the node number to control the sequencer
    
    void setNodeNumber(int number){
        nodeNumber = number;
    }

    void setSequencer(Sequencer *sequencer){
        currentSequencer = sequencer;
    }
    
    void checkNote(){
        if (currentSequencer->getCurrentNode() == nodeNumber) {
            currentNode = 1.0;
        } else {
            currentNode = 0.0;
        }
        repaint();
    }
    
private:
    ComponentBoundsConstrainer constrainer;
    ComponentDragger dragger;
    
    Sequencer *currentSequencer;

    int nodeNumber;
    int onOff;
    int currentSample;
    int currentNode;
    int newY;
    int dragY;
    int dragging;
    float yPos;
};


#endif  // NODE_H_INCLUDED
