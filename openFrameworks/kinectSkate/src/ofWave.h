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
    void setup(ofPoint center, long startTime, int numberWaveFronts, float velocity);
    void update();
    void draw();
    
    ofPoint center;
    ofColor color;
    int numberWaveFronts;
    float velocity;
    //float thickness;
    long startTime;
    long dt;
    long lastTime;
};