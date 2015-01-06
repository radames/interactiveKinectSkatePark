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
    void setup(ofPoint center, long startTime, int numberWaveFronts, float velocity, int _hue);
    void update();
    void draw();
    
    ofPoint center;
    int hue;
    int numberWaveFronts;
    float velocity;
    float accel;
    float linearVelocity;
    //float thickness;
    long startTime;
    long dt;
    long lastTime;
};