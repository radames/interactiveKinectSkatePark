#pragma once
#include "ofMain.h"
#include "ofParticles.h"


class ofBackEffects{
    
public:
    ofBackEffects();
    ~ofBackEffects();
    
    void setup();
    void reset();
    void update();
    void draw();

private:
    //background particles
    
    void resetParticles();

    vector <ofParticles> p;
    vector <ofPoint> attractPoints;
    vector <ofPoint> attractPointsWithMovement;
    
    particleMode currentMode;
    string currentModeStr;
    


};