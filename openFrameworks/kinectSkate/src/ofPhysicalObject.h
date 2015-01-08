//
//  ofPhysicalObject.h
//  kinectSkate
//
//  Created by Edgar Zanella on 1/6/15.
//
//

#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "ofAppConfig.h"
#include "ofxTwistedRibbon.h"

class ofPhysicalObject {
    
public:
    
    ofPhysicalObject();
    ~ofPhysicalObject();

    void setup(AppConfig *_appConfig, ofxBox2d *_box2d, ofVec2f velocity, ofPoint position, int _kinectNumber, int _label, int _width, int _height);
    void update();
    void updateVelocity(ofVec2f _velocity);
    void draw();
    bool isReadyToDie();
    ofPtr<ofxBox2dRect> rectBody;
    ofxBox2d *box2d;
    int kinectNumber;
    int label;
    ofPtr<ofxTwistedRibbon> ribbon;
    ofColor ribbonColor;
    float ang;
    ofImage objectImage;
    
    AppConfig *appConfig;
    
    float firstTime;
    float decayTime;

};