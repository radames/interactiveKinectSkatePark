//
//  ofWave.h
//  kinectSkate
//
//  Created by Edgar Zanella on 1/5/15.
//
//

#pragma once
#include "ofMain.h"

class ofWave {
    
public:
    ofWave();
    ~ofWave();

    void setup(ofPoint center, int numberWaveFronts, float velocity, ofColor _c, long _dTime);
    void update();
    void draw();
    bool isReadyToDie();
    
    ofPoint center;
    ofColor wColor;
    int numberWaveFronts;
    float velocity;
    float accel;
    float linearVelocity;
    //float thickness;
    long startTime;
    long dt;
    long lastTime;
    long dTime; //duration time
};