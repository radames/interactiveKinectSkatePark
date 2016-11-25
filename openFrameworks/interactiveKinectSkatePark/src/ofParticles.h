#pragma once
#include "ofMain.h"

enum particleMode{
    PARTICLE_MODE_ATTRACT = 0,
    PARTICLE_MODE_REPEL,
    PARTICLE_MODE_NEAREST_POINTS,
};

enum animationMode{
    POINT = 0,
    TRAIL,
};

class ofParticles{
    
public:
    ofParticles();
    
    void setMode(particleMode newMode);
    void setAnimationMode(animationMode newMode);
    void setAttractPoints( vector <ofPoint> * attract );
    void setColor(ofColor c);
    void setDecayTime(long dTime);
    bool isReadyToDie();
    
    void reset();
    void update();
    void draw();
    
    ofPoint pos;
    ofPoint vel;
    ofPoint frc;
    
    float drag;
    float uniqueVal;
    float scale;
    
    long firstTime;
    long decayTime;

    particleMode mode;
    animationMode animMode;
    
    ofColor pColor;
    
    vector <ofPoint> * attractPoints; 
};