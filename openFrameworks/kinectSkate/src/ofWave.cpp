//
//  ofWave.cpp
//  kinectSkate
//
//  Created by Edgar Zanella on 1/5/15.
//
//

#include "ofWave.h"

void ofWave::setup(ofPoint _center, long _startTime, int _numberWaveFronts, float _velocity) {
    center = _center;
    startTime = _startTime;
    numberWaveFronts =  _numberWaveFronts;
    velocity = _velocity;
    accel = 0.2;
    linearVelocity = 0.5;
    
}

void ofWave::update() {
    long now = ofGetElapsedTimeMillis();
    dt = (now - startTime)/velocity;
}

void ofWave::draw() {
    for (int i = 0; i < numberWaveFronts; ++i) {
        ofPushStyle();
        ofPath circle;
        circle.setCircleResolution(500);
        circle.arc(center, i*accel*dt*dt + i*linearVelocity*dt, i*accel*dt*dt + i*linearVelocity*dt, 0, 360);
        circle.close();
        circle.arc(center, i*accel*dt*dt + i*linearVelocity*dt + 0.6*i*dt*dt/20, i*accel*dt*dt + i*linearVelocity*dt + 0.6*i*dt*dt/20, 0, 360);
        circle.setColor(ofColor(255*floor(i)/10 + 100.255*floor(i)/10  + 30,0, 0, 205*floor(i)/10.0 + 50));
        circle.draw();
        ofPopStyle();
    }
}