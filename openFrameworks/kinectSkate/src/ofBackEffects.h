#pragma once
#include "ofMain.h"
#include "ofxBox2d.h"
#include "ofParticles.h"


class ofBackEffects{
    
public:
    ofBackEffects();
    ~ofBackEffects();
    
    void setup();
    void reset();
    void update(vector <ofPtr<ofxBox2dRect> > boxes);
    void draw();
    void updateAttractPoints(vector <ofPtr<ofxBox2dRect> > boxes);
    void addAttractPoints(ofPoint pos);
    void addParticles(int num, ofPoint origin, ofPoint velocity);
    
    ofParameterGroup particlesGUI;
    //maxParticles for the cycling list of particles
    ofParameter<int> maxParticles = 1500;
    ofParameter<ofColor>  particleColor;
    ofParameter<int>  decayTime;
    bool enableParticles = true;

private:
    //background particles
    
    void resetParticles();

    vector <ofParticles> particles;
    vector <ofPoint> attractPoints;
    vector <ofPoint> attractPointsWithMovement;

    particleMode currentMode;
    animationMode animMode;
    string currentModeStr;
    


};