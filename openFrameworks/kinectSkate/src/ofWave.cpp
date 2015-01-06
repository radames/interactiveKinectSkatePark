//
//  ofWave.cpp
//  kinectSkate
//
//  Created by Edgar Zanella on 1/5/15.
//
//

#include "ofWave.h"

ofWave::ofWave(){};
ofWave::~ofWave(){};


void ofWave::setup(ofPoint _center, int _numberWaveFronts, float _velocity , int _hue) {
    center = _center;
    startTime =  ofGetElapsedTimeMillis();
    numberWaveFronts =  _numberWaveFronts;
    velocity = _velocity;
    accel = 0.2;
    linearVelocity = 0.5;
    hue = _hue;
    dTime = 1000;
}

void ofWave::update() {
    long now = ofGetElapsedTimeMillis();
    dt = (now - startTime)/velocity;
}

void ofWave::draw() {
    float time = (ofGetElapsedTimeMillis() - startTime);
    for (int i = 0; i < numberWaveFronts; ++i) {
        ofPushStyle();
        ofPath circle;
        circle.setCircleResolution(500);
        circle.arc(center, i*accel*dt*dt + i*linearVelocity*dt, i*accel*dt*dt + i*linearVelocity*dt, 0, 360);
        circle.close();
        circle.arc(center, i*accel*dt*dt + i*linearVelocity*dt + 0.6*i*dt*dt/20, i*accel*dt*dt + i*linearVelocity*dt + 0.6*i*dt*dt/20, 0, 360);
        ofColor c;
        c.setHsb(hue,100 * floor(i)/10 + 155, 255, 255 - MIN(1,float(time)/dTime)*255);
        circle.setColor(c);
        circle.draw();
        ofPopStyle();
    }
}

bool ofWave::isReadyToDie(){
    if((ofGetElapsedTimeMillis() - startTime) > dTime){
        return true;
    }else{
        return false;
    }
}