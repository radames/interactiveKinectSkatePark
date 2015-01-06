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
}

void ofPhysicalObject::draw() {
    ofPushStyle();
    ofFill();
    ofSetHexColor(0xe63b8b);
    rectBody.get()->draw();
    ofPopStyle();
}