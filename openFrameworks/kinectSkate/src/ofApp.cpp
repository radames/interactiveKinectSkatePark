#include "ofApp.h"
#include "ofxSyphon.h"

using namespace cv;
using namespace ofxCv;

//--------------------------------------------------------------
void ofApp::setup() {

    ofRectangle bounds = ofRectangle(0, 0, CWIDTH, CHEIGHT);
    
    screenSetup(); //screen and some OF setups
    kinectSetup(1,"A00367813858042A"); //kinetic setup
    kinectSetup(0,""); //kinetic setup


    guiSetup(); //GUI Setup

    // register the listener so that we get the events
	ofAddListener(box2d.contactStartEvents, this, &ofApp::contactStart);
	ofAddListener(box2d.contactEndEvents, this, &ofApp::contactEnd);
    
    box2d.init();
    box2d.enableEvents();
	box2d.setGravity(0, 0);
	box2d.setFPS(30.0);
    box2d.createBounds(bounds);
    
}

//--------------------------------------------------------------
void ofApp::contactStart(ofxBox2dContactArgs &e) {
    cout << "CONTATO" << endl;
	if(e.a != NULL && e.b != NULL) {
		
		// if we collide with the ground we do not
		// want to play a sound. this is how you do that
		if(e.a->GetType() == b2Shape::e_circle && e.b->GetType() == b2Shape::e_circle) {
			
			ObjectData * aData = (ObjectData*)e.a->GetBody()->GetUserData();
			ObjectData * bData = (ObjectData*)e.b->GetBody()->GetUserData();
			
			if(aData) {
				aData->hit = true;
				//sound[aData->soundID].play();
			}
			
			if(bData) {
				bData->hit = true;
				//sound[bData->soundID].play();
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::contactEnd(ofxBox2dContactArgs &e) {
	if(e.a != NULL && e.b != NULL) {
		
		ObjectData * aData = (ObjectData*)e.a->GetBody()->GetUserData();
		ObjectData * bData = (ObjectData*)e.b->GetBody()->GetUserData();
		
		if(aData) {
			aData->hit = false;
		}
		
		if(bData) {
			bData->hit = false;
		}
	}
}


void ofApp::drawPositions() {
    for(int i=0; i<boxes.size(); i++) {
        ofVec2f pos_center = boxes[i].get()->getPosition();

        for (int j = i + 1; j < boxes.size(); j++) {
            ofFill();
            ofSetHexColor(0xff0000);
            ofVec2f pos_j = boxes[j].get()->getPosition();
           //     cout << pos_j.squareDistance(pos_center) << endl;
                if (pos_j.squareDistance(pos_center)/100 < 1200) {
                    ofLine(pos_center.x, pos_center.y, pos_j.x, pos_j.y);
                }
        }
	}
    
}

void ofApp::createObjects() {
    int j = 0;
    
    RectTracker& tracker = contourFinder[j].getTracker();
    const vector<unsigned int>& newLabels = tracker.getNewLabels();
    const vector<unsigned int>& currentLabels = tracker.getCurrentLabels();
    
    /*
    for(int i = 0; i < currentLabels.size(); i++) {
        int label = currentLabels[i];
        const cv::Rect& current = tracker.getCurrent(label);
        
        ofVec2f velocity = toOf(tracker.getVelocity(label));

        cout << label << " - " << velocity << endl;
        
            if (addedObjs.count(label) == 0) {
                float w = ofRandom(4, 20);
                float h = ofRandom(4, 20);
                ofPoint center = ofPoint(current.x, current.y);

                boxes.push_back(ofPtr<ofxBox2dRect>(new ofxBox2dRect));
                ofxBox2dRect * rect = boxes.back().get();
                ofVec2f velocity = toOf(tracker.getVelocity(label));

                cout << velocity << endl;
                rect->setVelocity(velocity.x, velocity.y);
                rect->setPhysics(3.0, 0.53, 0.1);
                rect->setup(box2d.getWorld(), center.x, center.y, w, h);
                addedObjs[label] = boxes.size() - 1;
                cout << "ANOTADO" << label << endl;
            } else if (addedObjs[label] != -1) {
                ofVec2f velocity = toOf(tracker.getVelocity(i));
               //cout << "GIVE VEL " << velocity << endl;
                ofPtr<ofxBox2dRect> rect = boxes[addedObjs[label]];
                rect->setVelocity(velocity.x, velocity.y);
                addedObjs[label] = -1;
            }
    }*/
    
    for(int i=0; i < contourFinder[j].size(); i++){
        
        unsigned int label = contourFinder[j].getLabel(i);
        
        if (addedObjs.count(label) == 0) {
            float w = 20;
            float h = 20;
            ofPoint center = toOf(contourFinder[j].getCenter(i));
            
            ofPtr<ofxBox2dRect> box = ofPtr<ofxBox2dRect>(new ofxBox2dRect);
            
            boxes.push_back(box);
            ofxBox2dRect *rect = box.get();
            ofVec2f velocity = toOf(tracker.getVelocity(i));
            
            rect->setVelocity(velocity.x, velocity.y);
            rect->setPhysics(3.0, 0.53, 0.1);
            rect->setup(box2d.getWorld(), center.x, center.y, w, h);

            rect->setData(new ObjectData());
            ObjectData *objData = (ObjectData *)rect->getData();
            objData->w = velocity.x * w;
            objData->h = velocity.y * h;
            objData->hit = true;
            
            addedObjs[label] = boxes.size() - 1;
        }
        
        if(tracker.existsPrevious(label) && addedObjs[label] != -1) {
            
            ofVec2f velocity = toOf(tracker.getVelocity(i));
            ofPtr<ofxBox2dRect> rect = boxes[addedObjs[label]];
            
            if (velocity.x != 0 && velocity.y != 0) {
                
                rect->setData(new ObjectData());
                ObjectData *objData = (ObjectData *)rect->getData();
                objData->w = velocity.x*20;
                objData->h = velocity.y *20;
                objData->hit = true;
                
                rect->setVelocity(-1*velocity.x, velocity.y);
                addedObjs[label] == -1;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::update() {

    ofEnableAlphaBlending();
	ofBackground(255, 255, 255);
    kinectUpdate();
	box2d.update();

    //varre os blobs, checa

    RectTracker& tracker = contourFinder[0].getTracker();
//    for(int i = 0; i < contourFinder.size(); i++) {
//        unsigned int label = contourFinder.getLabel(i);
//
//           if(tracker.existsPrevious(label)) {
//               //caso o tracker j‡ existe checa qual o novo ID
//
//               const cv::Rect& previous = tracker.getPrevious(label);
//               const cv::Rect& current = tracker.getCurrent(label);
//               //tracker.getPre
//               //atualiza o hash com a posicao dos morphs
//
//               morphRender.morphs[label].updatePosition(current.x, current.y);
//
//           }
//
//    }

    const vector<unsigned int>& currentLabels = tracker.getCurrentLabels();
    const vector<unsigned int>& previousLabels = tracker.getPreviousLabels();
    const vector<unsigned int>& deadLabels = tracker.getDeadLabels();

    createObjects();

    //varrer deadLabels e procurar morphs e KILL them
    /*

    for(int i = 0; i < contourFinder.size(); i++) {
        unsigned int label = contourFinder.getLabel(i);

       // int label = currentLabels[i];
        const cv::Rect& current = tracker.getCurrent(label);


        if(tracker.existsPrevious(label)) {
           // update position Morph
            if (morphRender.morphs.count(label) > 0) {
                morphRender.morphs[label].updatePosition(current.x, current.y);
            }
        } 
    }*/

   /*
    //delete objetcs for deadlabels if they exist
    for(int i = 0; i < deadLabels.size(); i++) {
        if (morphRender.morphs.count(deadLabels[i]) > 0) {
            morphRender.deleteMorph(deadLabels[i]);
        }
    }

    for(int i = 0; i < currentLabels.size(); i++) {
        int label = currentLabels[i];
        const cv::Rect& current = tracker.getCurrent(label);

        if(tracker.existsPrevious(label)) {
            if (morphRender.morphs.count(label) > 0) {
                morphRender.morphs[label].updatePosition(applyOffsetX(current.x), applyOffsetY(current.y));
            } else {
                morphRender.addMorph(applyOffsetX(current.x), applyOffsetY(current.y), label);
            }
        }
    }
    */
    
    
    /*
    for(int i = 0; i < newLabels.size(); i++) {
        int label = newLabels[i];
        const cv::Rect& current = tracker.getCurrent(label);
        morphRender.addMorph(applyOffsetX(current.x), applyOffsetY(current.y), label);
    }*/

}

//--------------------------------------------------------------
void ofApp::draw() {
    ofClear(0, 0, 0, 0);

    if(bDebugMode){ debugMode(); }//draw debug mode
    

    for(int i=0; i<boxes.size(); i++) {
		ofFill();
		ofSetHexColor(0xe63b8b);
		boxes[i].get()->draw();
	}
    
    drawPositions();
    
    syphonServer.publishScreen(); //syphon screen

}


///DEBUG-MODE

void ofApp::debugMode(){

    //showing kinect stuffs
    
    //width height debug screens
    float w = 300;
    float h = 200;
    
    for(int j = 0; j < 2; j++){
            // draw from the live kinect
        kinect[j].draw(j * w, h, w, h);
        kinect[j].drawDepth(j * w, 0, w, h);
        //contourFinder[j].draw();

        ofPushMatrix();

            ofTranslate(j * w, h*2);
        
            //scale grayImage and contourPositions to a small screen
        
            ofScale(w/kinect[j].width,h/kinect[j].height);
        
            grayImage[j].draw(0,0);
            contourFinder[j].draw();
        
        ofPopMatrix();

        //loop through all blobs detected and draw the centroid and lables

        RectTracker& tracker = contourFinder[j].getTracker();
        
        for(int i=0; i < contourFinder[j].size(); i++){
            
            unsigned int label = contourFinder[j].getLabel(i);
            
            if(tracker.existsPrevious(label)) {

                ofPoint center = toOf(contourFinder[j].getCenter(i));
                ofPushStyle();
                ofSetColor(255,0,0);
                ofFill();
                ofPushMatrix();
                    ofTranslate(j * w,h*2);
                    ofScale(w/kinect[i].width,h/kinect[i].height);
                    ofEllipse(center.x,center.y,10,10);
                    string msg = ofToString(label) + ":" + ofToString(tracker.getAge(label));
                    ofDrawBitmapString(msg,center.x,center.y);
                    ofVec2f velocity = toOf(contourFinder[j].getVelocity(i));
                    ofPushMatrix();
                        ofTranslate(center.x, center.y);
                        ofScale(10, 10);
                        ofLine(0, 0, velocity.x, velocity.y);
                        ofPopMatrix();
                    ofPopMatrix();
                ofPopMatrix();
                ofPopStyle();
            }

        }
    }
    // draw instructions
    ofPushStyle();

    ofSetColor(255, 255, 255);
    stringstream reportStream;


    ofDrawBitmapString(reportStream.str(), 20, 652);
    ofPopStyle();
    gui.draw();



}


//--------------------------------------------------------------

void ofApp::screenSetup(){

    syphonServer.setName("kinectTracking");

    ofSetWindowShape(CHEIGHT,CWIDTH); //set windowSize the same as the


    //starting FBO buffers for each screen
    screen1.allocate(CWIDTH, CHEIGHT,  GL_RGBA32F_ARB);

    screen1.begin();
    ofClear(255,255,255, 0);
    screen1.end();


    ofEnableSmoothing();
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_NOTICE);

}

void ofApp::kinectUpdate(){

    for(int i = 0; i < 2; i++){
        kinect[i].update();
        // there is a new frame and we are connected
        if(kinect[i].isFrameNew()) {

            // load grayscale depth image from the kinect source
            grayImage[i].setFromPixels(kinect[i].getDepthPixels(), kinect[i].width, kinect[i].height);

            // we do two thresholds - one for the far plane and one for the near plane
            // we then do a cvAnd to get the pixels which are a union of the two thresholds
            grayThreshNear[i] = grayImage[i];
            grayThreshFar[i] = grayImage[i];
            grayThreshNear[i].threshold(nearThreshold, true);
            grayThreshFar[i].threshold(farThreshold[i]);
            cvAnd(grayThreshNear[i].getCvImage(), grayThreshFar[i].getCvImage(), grayImage[i].getCvImage(), NULL);


            // update the cv images
            grayImage[i].flagImageChanged();

            contourFinder[i].setMinAreaRadius(minBlobSize[i]);
            contourFinder[i].setMaxAreaRadius(maxBlobSize[i]);
            contourFinder[i].findContours(grayImage[i]);
        }
    }

}
void ofApp::kinectSetup(int kinectNumber, string id){
    
    //A00367813858042A

    // enable depth.video image calibration
    kinect[kinectNumber].setRegistration(true);

    kinect[kinectNumber].init();
    
    if(id.empty()){
        kinect[kinectNumber].open();
    }else{
        kinect[kinectNumber].open(id);
    }

    //kinect.init(true); // shows infrared instead of RGB video image
    //kinect.init(false, false); // disable video image (faster fps)


    // print the intrinsic IR sensor values
    if(kinect[kinectNumber].isConnected()) {
        ofLogNotice() << "Kinect " << id << " Connected";
        ofLogNotice() << "sensor-emitter dist: " << kinect[kinectNumber].getSensorEmitterDistance() << "cm";
        ofLogNotice() << "sensor-camera dist:  " << kinect[kinectNumber].getSensorCameraDistance() << "cm";
        ofLogNotice() << "zero plane pixel size: " << kinect[kinectNumber].getZeroPlanePixelSize() << "mm";
        ofLogNotice() << "zero plane dist: " << kinect[kinectNumber].getZeroPlaneDistance() << "mm";
    }
    

    colorImg[kinectNumber].allocate(kinect[kinectNumber].width, kinect[kinectNumber].height);
    grayImage[kinectNumber].allocate(kinect[kinectNumber].width, kinect[kinectNumber].height);
    grayThreshNear[kinectNumber].allocate(kinect[kinectNumber].width, kinect[kinectNumber].height);
    grayThreshFar[kinectNumber].allocate(kinect[kinectNumber].width, kinect[kinectNumber].height);

    
    ofSetFrameRate(60);
    // zero the tilt on startup
    kinect[kinectNumber].setCameraTiltAngle(0);

    nearThreshold = 255;
    
    //blob tracking system parameter
    contourFinder[kinectNumber].getTracker().setPersistence(10);
    contourFinder[kinectNumber].getTracker().setMaximumDistance(128);
    
}



void ofApp::guiSetup(){

    gui.setup("Settings", "settings.xml", 310,100);

    gui.add(enableMouse.set("Mouse DEBUG",true));
    
    for(int i = 0; i < 2; i++){
    
        parametersKinect[i].setName("Kinect " + ofToString(i));
        parametersKinect[i].add(farThreshold[i].set("Far Threshold", 0,0, 255 ));
        parametersKinect[i].add(numMaxBlobs[i].set("Num Max Blos",10,0,15));
        parametersKinect[i].add(maxBlobSize[i].set("max Blob Size",0,0,500));
        parametersKinect[i].add(minBlobSize[i].set("min Blob Size",0,0,500));
        parametersKinect[i].add(offsetX[i].set("Offset X", 0,-200, 200 ));
        parametersKinect[i].add(offsetY[i].set("Offset Y", 0,-200, 200 ));
        gui.add(parametersKinect[i]);

    }
    gui.minimizeAll();
    // events for change in paramenters on ofpp application

    gui.loadFromFile("settings.xml");

}
//Gui events for kinect Area
void ofApp::kinectUpdateAreaW(int& kinectWidth){

}

void ofApp::kinectUpdateAreaH(int& kinectHeight){

}


float ofApp::applyOffsetX(float _x){
    return;
    //return _x + offsetX;

}
float ofApp::applyOffsetY(float _y){
    return;
    //return _y + offsetY;
}



//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
	switch (key) {

        case 'i':
            break;
            
        case 's':
            gui.saveToFile("settings.xml");

            break;
        case 'l':
            gui.loadFromFile("settings.xml");
            break;

        case 'f':
            ofToggleFullscreen();
            ofSetWindowShape(CWIDTH,CHEIGHT); //set windowSize the same as the
            break;

        case 'd':
            bDebugMode = !bDebugMode;
            break;
            
		case 'm':
            enableMouse = !enableMouse;
			break;

		case OF_KEY_UP:
            bloby-=10;

			break;

		case OF_KEY_DOWN:
            bloby+=10;
			break;

        case OF_KEY_LEFT:
            blobx-=10;
            break;

        case OF_KEY_RIGHT:
            blobx+=10;
            break;

        case 'z':
            break;
        case 'x':

            break;


	}



}
void ofApp::mouseMoved(int x, int y){
    if(enableMouse){
        
    }

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{}


void ofApp::exit() {
    kinect[0].close();
    kinect[1].close();
}
