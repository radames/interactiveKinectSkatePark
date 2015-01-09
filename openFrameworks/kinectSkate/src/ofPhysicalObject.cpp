//
//  ofPhysicalObject.cpp
//  kinectSkate
//
//  Created by Edgar Zanella on 1/6/15.
//
//

#include "ofPhysicalObject.h"

ofPhysicalObject::ofPhysicalObject(){};
ofPhysicalObject::~ofPhysicalObject(){};


void ofPhysicalObject::setup(AppConfig *_appConfig, ofxBox2d *_box2d, ofVec2f velocity, ofPoint position, int _kinectNumber, int _label, int _width, int _height) {
    appConfig = _appConfig;
    label = _label;
    kinectNumber = _kinectNumber;
    box2d = _box2d;
    rectBody = ofPtr<ofxBox2dRect>(new ofxBox2dRect);
    ofxBox2dRect *rect = rectBody.get();
    rect->setVelocity(velocity.x, velocity.y);
    rect->setPhysics(3.0, 0.53, 0.1);
    rect->setup(box2d->getWorld(), position.x, position.y, _width, _height);

    ofVec3f ribPos;
    ribPos.z = 0;
    ribPos.x  = position.x;
    ribPos.y  = position.y;
    ofColor color;
    int hue = ofRandom(0, 255);
    ribbonColor.setHsb(hue, 120, 100, 150);

    ribbon = ofPtr<ofxTwistedRibbon>(new ofxTwistedRibbon);
    ribbon->update(position, color);
    ribbon->length  = ofRandom(50, 200);
    ang = ofRandom(5, 30);

    objectImage.loadImage("golden.png");

    firstTime = ofGetElapsedTimeMillis();
    decayTime = 5000;

}

void ofPhysicalObject::setDecayTime(long _dTime){
    decayTime = _dTime;
}

void ofPhysicalObject::updateVelocity(ofVec2f _velocity) {
    rectBody->setVelocity(-_velocity.y, _velocity.x);
}

void ofPhysicalObject::update() {

    ofPoint newRectPosition = rectBody.get()->getPosition();
    ofVec3f newPosition;
    float radius = sin(ofGetElapsedTimef()) * 50 + 200;
    newPosition.z = 0; //sin(ofGetElapsedTimef() * 0.3) * radius;
    newPosition.x  = newRectPosition.x;
    newPosition.y  = newRectPosition.y;
    ofColor color;
    int sat = int(ofGetElapsedTimef() * 10) % 255;
    color.setHsb(ribbonColor.getHue(), 255, 255, 255);
    ribbon->update(newPosition, color);
}

void ofPhysicalObject::draw() {
    if (appConfig->runningMode == TRAILS) {
        ofPushStyle();
        //ofFill();
        //ofSetHexColor(0xe63b8b);
        //rectBody.get()->draw();
        ofPushMatrix();
        //ofRotate(ofGetElapsedTimef() * ang, 1, 1, 0);
        ribbon->draw();
        ofPopMatrix();
        ofPopStyle();
    }

    if (appConfig->runningMode == SHAPES) {
        ofPushStyle();
        ofPoint pos = rectBody->getPosition();
        float ang = rectBody->getRotation();
        ofPushMatrix();
        ofTranslate(pos.x, pos.y);
        ofRotate(ang, 0, 0, 1);
        objectImage.draw(-25, -25);
        ofPopMatrix();
        ofPopStyle();
    }
    
    if (appConfig->runningMode == SQUARES) {
        ofPushStyle();
        ofNoFill();
        ofPoint pos = rectBody->getPosition();
        ofSetLineWidth(10);
        //ofRect(pos.x, pos.y, 50, 50);
        rectBody->draw();
        ofPopStyle();
    }
}

bool ofPhysicalObject::isReadyToDie(){
    if((ofGetElapsedTimeMillis() - firstTime) > decayTime){
        return true;
    }else{
        return false;
    }
}
