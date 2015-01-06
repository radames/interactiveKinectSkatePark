//
//  ofPhysicalObject.cpp
//  kinectSkate
//
//  Created by Edgar Zanella on 1/6/15.
//
//

#include "ofPhysicalObject.h"

void ofPhysicalObject::setup(ofxBox2d *_box2d, ofVec2f velocity, ofPoint position, int _kinectNumber, int _label, int _width, int _height) {
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
    
    ribbon = ofPtr<ofxTwistedRibbon>(new ofxTwistedRibbon);
    ribbon->update(position, color);
}

void ofPhysicalObject::updateVelocity(ofVec2f _velocity) {
    rectBody->setVelocity(-_velocity.y, _velocity.x);
}

void ofPhysicalObject::update() {
    
    ofPoint newRectPosition = rectBody.get()->getPosition();
    ofVec3f newPosition;
    float radius = sin(ofGetElapsedTimef()) * 50 + 200;
    newPosition.z = sin(ofGetElapsedTimef() * 0.3) * radius;
    newPosition.x  = newRectPosition.x;
    newPosition.y  = newRectPosition.y;
    ofColor color;
    int hue = int(ofGetElapsedTimef() * 10) % 255;
    color.setHsb(hue, 120, 255);
    ribbon->update(newPosition, color);
}

void ofPhysicalObject::draw() {
    ofPushStyle();
    ofFill();
    ofSetHexColor(0xe63b8b);
    rectBody.get()->draw();
    ribbon->draw();
    ofPopStyle();
}