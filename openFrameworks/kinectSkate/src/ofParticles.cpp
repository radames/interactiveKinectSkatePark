#include "ofParticles.h"

//------------------------------------------------------------------
ofParticles::ofParticles(){
    attractPoints = NULL;
    firstTime = ofGetElapsedTimeMillis();
    decayTime = ofRandom(1000,5000);
    pColor = ofColor(ofRandom(255),ofRandom(255),ofRandom(255));
}

void ofParticles::setDecayTime(long dTime){
    decayTime = dTime;
    
}
//------------------------------------------------------------------
void ofParticles::setMode(particleMode newMode){
    mode = newMode;
}

//------------------------------------------------------------------
void ofParticles::setAnimationMode(animationMode newMode){
    animMode = newMode;
}

//------------------------------------------------------------------
void ofParticles::setAttractPoints( vector <ofPoint> * attract ){
    attractPoints = attract;
}

//------------------------------------------------------------------
void ofParticles::reset(){
    //the unique val allows us to set properties slightly differently for each particle
    uniqueVal = ofRandom(-10000, 10000);
    
    pos.x = ofRandomWidth();
    pos.y = ofRandomHeight();
    
    vel.x = ofRandom(-3.9, 3.9);
    vel.y = ofRandom(-3.9, 3.9);
    
    frc   = ofPoint(0,0,0);
    
    scale = ofRandom(0.5, 1.0);

    drag  = ofRandom(0.95, 0.998);
    
}

//------------------------------------------------------------------
void ofParticles::update(){
    
    //1 - APPLY THE FORCES BASED ON WHICH MODE WE ARE IN
    
    if( mode == PARTICLE_MODE_ATTRACT ){
        ofPoint attractPt(ofGetMouseX(), ofGetMouseY());
        frc = attractPt-pos; // we get the attraction force/vector by looking at the mouse pos relative to our pos
        frc.normalize(); //by normalizing we disregard how close the particle is to the attraction point
        vel *= drag; //apply drag
        vel += frc * 0.6; //apply force
        
    }
    else if( mode == PARTICLE_MODE_REPEL ){
        if( attractPoints ){
            
            //1 - find closest attractPoint
            ofPoint closestPt;
            int closest = -1;
            float closestDist = 9999999;
            
            for(unsigned int i = 0; i < attractPoints->size(); i++){
                float lenSq = ( attractPoints->at(i)-pos ).lengthSquared();
                if( lenSq < closestDist ){
                    closestDist = lenSq;
                    closest = i;
                }
            }
            
            float dist = frc.length();

            if( closest != -1 ){
                closestPt = attractPoints->at(closest);
                float dist = sqrt(closestDist);
                
                //in this case we don't normalize as we want to have the force proportional to distance
                frc = closestPt - pos;
                
                vel *= drag;
                
                //lets also limit our attraction to a certain distance and don't apply if 'f' key is pressed
                if( dist < 300  && !ofGetKeyPressed('F')){
                    vel += -frc * 0.003;
                }else{
                    //if the particles are not close to us, lets add a little bit of random movement using noise. this is where uniqueVal comes in handy.
                    frc.x = ofSignedNoise(uniqueVal, pos.y * 0.01, ofGetElapsedTimef()*0.2);
                    frc.y = ofSignedNoise(uniqueVal, pos.x * 0.01, ofGetElapsedTimef()*0.2);
                    vel += frc * 0.4;
                }
                
            }
            
        }
        
    }
    else if( mode == PARTICLE_MODE_NEAREST_POINTS ){
        
        if( attractPoints ){
            
            //1 - find closest attractPoint
            ofPoint closestPt;
            int closest = -1;
            float closestDist = 9999999;
            
            for(unsigned int i = 0; i < attractPoints->size(); i++){
                float lenSq = ( attractPoints->at(i)-pos ).lengthSquared();
                if( lenSq < closestDist ){
                    closestDist = lenSq;
                    closest = i;
                }
            }
            
            //2 - if we have a closest point - lets calcuate the force towards it
            if( closest != -1 ){
                closestPt = attractPoints->at(closest);
                float dist = sqrt(closestDist);
                
                //in this case we don't normalize as we want to have the force proportional to distance
                frc = closestPt - pos;
                
                vel *= drag;
                
                //lets also limit our attraction to a certain distance and don't apply if 'f' key is pressed
                if( dist < 300 && dist > 40 && !ofGetKeyPressed('f') ){
                    vel += frc * 0.003;
                }else{
                    //if the particles are not close to us, lets add a little bit of random movement using noise. this is where uniqueVal comes in handy.
                    frc.x = ofSignedNoise(uniqueVal, pos.y * 0.01, ofGetElapsedTimef()*0.2);
                    frc.y = ofSignedNoise(uniqueVal, pos.x * 0.01, ofGetElapsedTimef()*0.2);
                    vel += frc * 0.4;
                }
                
            }
            
        }
        
    }
    
    
    //2 - UPDATE OUR POSITION
    
    pos += vel;
    
    
    //3 - (optional) LIMIT THE PARTICLES TO STAY ON SCREEN
    //we could also pass in bounds to check - or alternatively do this at the ofApp level
    if( pos.x > ofGetWidth() ){
        pos.x = ofGetWidth();
        vel.x *= -1.0;
    }else if( pos.x < 0 ){
        pos.x = 0;
        vel.x *= -1.0;
    }
    if( pos.y > ofGetHeight() ){
        pos.y = ofGetHeight();
        vel.y *= -1.0;
    }
    else if( pos.y < 0 ){
        pos.y = 0;
        vel.y *= -1.0;
    }	
    
}

//------------------------------------------------------------------
void ofParticles::draw(){
    ofPushStyle();
    switch (animMode) {
        case POINT:
            ofSetColor(pColor);
            ofDrawCircle(pos.x, pos.y, scale * 4.0);
            
            break;
        
        case TRAIL:
            ofSetColor(pColor);
            // colour, pow(r,0.1), 0.9*sqrt(1-r), Z[i].magnitude/100+abs(Z[i].z/depth)*0.05
            //cout << vel.length() << endl;
            //ofSetColor(ofColor::fromHsb(128, 255, 255, 1));
            ofSetLineWidth(20);
            ofDrawLine(pos.x, pos.y, -vel.length()/12*30, pos.x - 4*vel.x, pos.y - 4*vel.y, 0);
            //ofDrawCircle(pos.x, pos.y, scale * 4.0);
            break;
    }
    ofPopStyle();
}

bool ofParticles::isReadyToDie(){
    if((ofGetElapsedTimeMillis() - firstTime) > decayTime){
        return true;
    }else{
        return false;
    }
}

void ofParticles::setColor(ofColor c){
    pColor = c;
}

