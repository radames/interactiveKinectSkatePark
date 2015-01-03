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
    void addParticles(int num, ofPoint origin);
    
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