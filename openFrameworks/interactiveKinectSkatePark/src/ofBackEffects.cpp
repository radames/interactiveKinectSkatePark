#include "ofBackEffects.h"


//------------------------------------------------------------------
ofBackEffects::ofBackEffects(){}
ofBackEffects::~ofBackEffects(){}

//------------------------------------------------------------------
void ofBackEffects::setup(){

    //particles background
    currentMode = PARTICLE_MODE_NEAREST_POINTS;
    animMode = TRAIL;
   // particles.assign(0, ofParticles());

    resetParticles();
    
    particlesGUI.setName("Particles");
    particlesGUI.add(maxParticles.set("max NumParticles", 100, 10, 3000));
    particlesGUI.add(decayTime.set("decay Time", 2000,100,5000));
    particlesGUI.add(particleColor.set("Particle Color", ofColor(255,255), ofColor(0,0),ofColor(255,255)));

}

void ofBackEffects::addParticles(int num, ofPoint origin, ofPoint velocity) {

    
        for (int i = 0; i < num; ++i) {
            ofParticles p;
            p.setDecayTime(decayTime);
            p.setColor(particleColor);
            p.uniqueVal = ofRandom(-10000, 10000);
            p.pos.x = origin.x;
            p.pos.y = origin.y;
            
            p.vel.x = velocity.x*cos((float(i)/num) * 2*pi);
            p.vel.y = velocity.y*sin((float(i)/num) * 2*pi);
            
            // cout << "VELO " << velocity.x << "  " << velocity.y << endl;
            
            p.frc   = ofPoint(0,0,0);
            
            p.scale = ofRandom(0.5, 1.0);
            
            p.drag  = ofRandom(0.95, 0.998);
            p.setMode(currentMode);
            p.setAttractPoints(&attractPointsWithMovement);
     
            if(particles.size() < maxParticles){
                particles.push_back(p);
            }else{
                return;
            }
    }
}

void ofBackEffects::draw(){
    if(enableParticles){
        
        for(unsigned int i = 0; i < particles.size(); i++){
            particles[i].draw();
        }

    }
}

void ofBackEffects::update(vector <ofPtr<ofxBox2dRect> > boxes){


    for(vector<ofParticles>::iterator it = particles.begin(); it != particles.end(); ){
        it->setMode(currentMode);
        it->setAnimationMode(animMode);
        it->setDecayTime(decayTime);
        it->update();
        
        if(it->isReadyToDie()){
           it = particles.erase(it);
        }else{
            ++it;
        }
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

//    for(unsigned int i = 0; i < particles.size(); i++){
//        ofParticles p;
//        p.setMode(currentMode);
//        p.setAttractPoints(&attractPointsWithMovement);
//        p.reset();
//        particles[i] = p;
//    }

}
