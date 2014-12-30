#include "ofBackEffects.h"


//------------------------------------------------------------------
ofBackEffects::ofBackEffects(){}
ofBackEffects::~ofBackEffects(){}

//------------------------------------------------------------------
void ofBackEffects::setup(){

    //particles background
    currentMode = PARTICLE_MODE_NEAREST_POINTS;

    p.assign(1500, ofParticles());
    resetParticles();

}

void ofBackEffects::draw(){

    for(unsigned int i = 0; i < p.size(); i++){
        p[i].draw();
    }

    ofSetColor(190);
    if( currentMode == PARTICLE_MODE_NEAREST_POINTS ){
    /*
        for(unsigned int i = 0; i < attractPoints.size(); i++){
            ofNoFill();
//       cout << "A >  " << attractPointsWithMovement[i] << endl;
            ofCircle(attractPointsWithMovement[i], 10);
            ofFill();
            ofCircle(attractPointsWithMovement[i], 4);
        }*/
    }

}

void ofBackEffects::update(vector <ofPtr<ofxBox2dRect> > boxes){

    for(unsigned int i = 0; i < p.size(); i++){
        p[i].setMode(currentMode);
        p[i].update();
    }

    //lets add a bit of movement to the attract points
    for(unsigned int i = 0; i < attractPointsWithMovement.size(); i++){
        attractPointsWithMovement[i].x = attractPoints[i].x + ofSignedNoise(i * 10, ofGetElapsedTimef() * 0.7) * 12.0;
        attractPointsWithMovement[i].y = attractPoints[i].y + ofSignedNoise(i * -10, ofGetElapsedTimef() * 0.7) * 12.0;
    }

    updateAttractPoints(boxes);
}

void ofBackEffects::updateAttractPoints(vector <ofPtr<ofxBox2dRect> > boxes) {
    for (int i = 0; i < boxes.size(); ++i) {
        attractPoints[i] = boxes[i]->getPosition();
    }
    attractPointsWithMovement = attractPoints;
}

void ofBackEffects::addAttractPoints(ofPoint pos) {
    cout << "PONTO " << pos << endl ;
    attractPoints.push_back(pos);
    attractPointsWithMovement = attractPoints;
}

void ofBackEffects::resetParticles(){

    //these are the attraction points used in the forth demo
    attractPoints.clear();
    for(int i = 0; i < 4; i++){
       // attractPoints.push_back( ofPoint( ofMap(i, 0, 4, 100, ofGetWidth()-100) , ofRandom(100, ofGetHeight()-100) ) );
    }

    attractPointsWithMovement = attractPoints;

    for(unsigned int i = 0; i < p.size(); i++){
        p[i].setMode(currentMode);
        p[i].setAttractPoints(&attractPointsWithMovement);;
        p[i].reset();
    }

}
