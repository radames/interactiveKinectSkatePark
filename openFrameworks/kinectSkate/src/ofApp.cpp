#include "ofApp.h"
#include "ofxBox2d.h"
#include "ofxSyphon.h"

using namespace cv;
using namespace ofxCv;

//--------------------------------------------------------------
void ofApp::setup() {
    
    post.init(CWIDTH, CHEIGHT);
    post.createPass<BloomPass>()->setEnabled(false);
    post.createPass<DofPass>()->setEnabled(false);
    post.createPass<KaleidoscopePass>()->setEnabled(false);
    post.createPass<NoiseWarpPass>()->setEnabled(false);
    post.createPass<PixelatePass>()->setEnabled(false);
    post.createPass<GodRaysPass>()->setEnabled(false);
    post.createPass<EdgePass>()->setEnabled(false);
    post.createPass<FxaaPass>()->setEnabled(false);

    appConfig.runningMode = TRAILS;
    
	trail_i.assign(100, 0);
	ofRectangle bounds = ofRectangle(0, 0, CWIDTH, CHEIGHT);

	screenSetup(); //screen and some OF setups
	kinectSetup(1,"A00367813858042A"); //kinetic setup
	kinectSetup(0,""); //kinetic setup

	// register the listener so that we get the events
	ofAddListener(box2d.contactStartEvents, this, &ofApp::contactStart);
	ofAddListener(box2d.contactEndEvents, this, &ofApp::contactEnd);

	box2d.init();
	box2d.enableEvents();
	box2d.setGravity(0, 0);
	box2d.setFPS(30.0);
	box2d.createBounds(bounds);

	myBack.setup();
	ofSetVerticalSync(false);
	ofSetFrameRate(60);

	// dictionaries to control already added objects based on tracker labels
	tr1::unordered_map<int,int> ao1, ao2;
	addedObjs.push_back(ao1);
	addedObjs.push_back(ao2);

	debugImage.loadImage("skatepark.png");
	debugImage.resize(1024*2, 768);

	//Osc communication
	sender.setup(HOST, PORT);
	receiver.setup(PORT);

	//ribbon = new ofxTwistedRibbon();

	guiSetup(); //GUI Setup
    
    objectImage.loadImage("data/circle.png");

}

//--------------------------------------------------------------
void ofApp::contactStart(ofxBox2dContactArgs &e) {
	if(e.a != NULL && e.b != NULL) {
		if(e.a->GetType() == b2Shape::e_polygon && e.b->GetType() == b2Shape::e_polygon) {

			ObjectData * aData = (ObjectData*)e.a->GetBody()->GetUserData();
			ObjectData * bData = (ObjectData*)e.b->GetBody()->GetUserData();


			ContactData c;
			c.x = e.a->GetBody()->GetPosition().x;
			c.y = e.a->GetBody()->GetPosition().y;
			c.r = 10;

			colCenters.push_back(c);

			if(aData) {
				aData->hit = true;
			}

			if(bData) {
				bData->hit = true;
			}

			myBack.addParticles(50, ofPoint(OFX_BOX2D_SCALE*e.a->GetBody()->GetPosition().x, OFX_BOX2D_SCALE*e.a->GetBody()->GetPosition().y),
					ofPoint(e.a->GetBody()->GetLinearVelocity().x, e.a->GetBody()->GetLinearVelocity().y));
			myBack.addParticles(50, ofPoint(OFX_BOX2D_SCALE*e.b->GetBody()->GetPosition().x, OFX_BOX2D_SCALE*e.b->GetBody()->GetPosition().y),
					ofPoint(e.b->GetBody()->GetLinearVelocity().x, e.b->GetBody()->GetLinearVelocity().y));
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
			if (pos_j.squareDistance(pos_center)/100 < 1200) {
				ofLine(pos_center.x, pos_center.y, pos_j.x, pos_j.y);
			}
		}
	}

	for (int i = 0; i < colCenters.size(); ++i) {
		ContactData c = colCenters[i];

		ofCircle(c.x, c.y, 0, c.r*20);
	}

}

void ofApp::updateTrail() {
	for(int i=0; i < boxes.size(); i++) {
		ofPoint newPosition = boxes[i].get()->getPosition();
		int old_trail = (trail_i[i] > 0)?trail_i[i] - 1:30;

		if ((newPosition - trail[i][old_trail]).length() > 10) {
			trail[i][trail_i[i]] = newPosition;
			trail_i[i] = (trail_i[i] + 1) % 30;
		}
	}
}

void ofApp::createObjects() {

	for(int kinectNumber = 0; kinectNumber < 2; kinectNumber++){
		//loop nas kinects

		RectTracker& tracker = contourFinder[kinectNumber].getTracker();
		const vector<unsigned int>& newLabels = tracker.getNewLabels();
		const vector<unsigned int>& currentLabels = tracker.getCurrentLabels();

		for(int i=0; i < contourFinder[kinectNumber].size(); i++){

			unsigned int label = contourFinder[kinectNumber].getLabel(i);

			if (addedObjs[kinectNumber].count(label) == 0) {
				float w = 50;
				float h = 50;
				ofPoint center = toWorldCoord(toOf(contourFinder[kinectNumber].getCenter(i)), kinectNumber);
                
				// Create new wave
                ofWave newWave;
                newWave.setup(center, 10, 50, waveColor[kinectNumber], waveDecayTime[kinectNumber]);
				waves.push_back(newWave);

				// Add Physical Object
				ofVec2f velocity = toOf(tracker.getVelocity(i));
                ofPhysicalObject physicalObject;
				physicalObject.setup(&appConfig, &box2d, velocity*velocityRatio, center, kinectNumber, label, w, h);
                physicalObject.setDecayTime(pObjDecTime);
				physObjects.push_back(physicalObject);

                // Keep track of object index
                addedObjs[kinectNumber][label] = physObjects.size() - 1;
                
				// Add trail to the new object

				/*
				   vector <ofPoint> box_trail;
				   box_trail.assign(30, ofPoint());
				   for (int ti = 0; ti < 30; ++ti) {
                        box_trail[ti] = center;
				   }
				   trail.push_back(box_trail);
                */
                
				// Add attract points to background
				myBack.addAttractPoints(center);

				//Osc Message for new Objects on the screen based on the sensorPositions[j] j = kinectic number
				ofxOscMessage m;

				if(center.x  < sensorPos[kinectNumber]-> x ){ //if this then he've appeared first on th left
					m.setAddress("/skatista/ED");
				}else{
					m.setAddress("/skatista/DE");
				}
				m.addIntArg(kinectNumber); // which sensor plus velocity
				sender.sendMessage(m);

			}

			if (tracker.existsPrevious(label) && addedObjs[kinectNumber][label] != -1) {
				ofVec2f velocity = toOf(tracker.getVelocity(i));
				if (velocity.x != 0 && velocity.y != 0) {
                    ofPhysicalObject physObject = physObjects[addedObjs[kinectNumber][label]];
                    physObject.updateVelocity(velocity * velocityRatio);
					addedObjs[kinectNumber][label] == -1;
				}
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::update() {
	kinectUpdate();
	box2d.update();

	myBack.update(boxes);

	//oscUpdate();
	//varre os blobs, checa
	RectTracker& tracker = contourFinder[0].getTracker();
	const vector<unsigned int>& currentLabels = tracker.getCurrentLabels();
	const vector<unsigned int>& previousLabels = tracker.getPreviousLabels();
	const vector<unsigned int>& deadLabels = tracker.getDeadLabels();

	createObjects();
	updateTrail();

	long now = ofGetElapsedTimeMillis();

	if (now - lastTime > 500) {
		box2d.setGravity(ofRandom(-10, 10), ofRandom(-10, 10));
		lastTime = now;
	}

    // Update waves
    for(vector<ofWave>::iterator it = waves.begin(); it != waves.end(); ){
        it->update();
        if(it->isReadyToDie()){
            it = waves.erase(it);
        }else{
            ++it;
        }
    }

    // Update Physical Objects
    for(vector<ofPhysicalObject>::iterator it = physObjects.begin(); it != physObjects.end(); ){
        it->update();
        if(it->isReadyToDie()){
            addedObjs[it->kinectNumber].erase(it->label); //delete from the dictionary of labels
            it = physObjects.erase(it); //delete from the vector list
        }else{
            ++it;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    
	glClearColor(0.0, 0.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //ofClear(0,0,0,255);
	ofEnableAlphaBlending();
  

    if (!bDebugMode) {
        post.begin();

        ofTranslate(CWIDTH/2, CHEIGHT/2);
        ofRotate(180, 0, 0, 1);
        ofRotate(180, 0, 1, 0);
        ofTranslate(-CWIDTH/2, -CHEIGHT/2);
    }
    
    if(bDebugMode){ debugMode(); }//draw debug mode


	for (int i = 0; i < physObjects.size(); ++i) {
		physObjects[i].draw();
	}
    
    if (appConfig.runningMode == GRAPH) {
        ofPushStyle();
        for (int i = 0; i < physObjects.size(); ++i) {
            for (int j = i + 1; j < physObjects.size(); ++j) {
                ofPoint p1 = physObjects[i].rectBody->getPosition();
                ofPoint p2 = physObjects[j].rectBody->getPosition();
                ofSetLineWidth(20);
                ofLine(p1.x, p1.y, p2.x, p2.y);
            }
        }
        ofPopStyle();

    }
    
    
    // drawPositions();
    myBack.draw(); //draw background effects

    // draw objects trail
    drawTrail();
    // draw waves
    for (int i = 0; i < waves.size(); ++i) {
        waves[i].draw();
    }

   if (!bDebugMode) {
        post.end();
    }
    syphonServer.publishScreen(); //syphon screen
}

void ofApp::drawTrail() {

	for (int i = 0; i < boxes.size(); ++i) {
		for (int j = 0; j < trail[i].size() - 1; j++) {
			ofPushStyle();
			ofPoint p1 = trail[i][(trail_i[i] + j) % 30];
			ofPoint p2 = trail[i][(trail_i[i] + j + 1) % 30];
			ofSetColor(255, 0, 0);
			//  ofSetLineWidth(j*2);
			ofLine(p1.x, p1.y, p2.x, p2.y);
			ofPopStyle();
		}
	}
}

///DEBUG-MODE

void ofApp::debugMode(){

	//showing kinect stuffs

	//width height debug screens
	float w = 300;
	float h = 200;
	debugImage.draw(0,0);

    for(int kinectNumber = 0; kinectNumber < 2; kinectNumber++){
		//drawing two depth areas

		ofPushMatrix();

		ofPushStyle();
		ofSetRectMode(OF_RECTMODE_CENTER);

		ofTranslate(sensorPos[kinectNumber]->x, sensorPos[kinectNumber]->y);
		ofScale(sensorArea[kinectNumber],sensorArea[kinectNumber]);
		ofRotate(90);
		kinect[kinectNumber].drawDepth(0,0,kinect[kinectNumber].width, kinect[kinectNumber].height);
		ofPushMatrix();
		ofTranslate(-kinect[kinectNumber].width/2,-kinect[kinectNumber].height/2);
		ofSetRectMode(OF_RECTMODE_CORNER);
		contourFinder[kinectNumber].draw();
		ofPopMatrix();
		ofPopStyle();
		ofPopMatrix();

		RectTracker& tracker = contourFinder[kinectNumber].getTracker();

		for(int i=0; i < contourFinder[kinectNumber].size(); i++){

			unsigned int label = contourFinder[kinectNumber].getLabel(i);

			if(tracker.existsPrevious(label)) {

				ofPoint center = toWorldCoord( toOf(contourFinder[kinectNumber].getCenter(i)),kinectNumber);
				ofPushStyle();
				ofSetColor(255,0,0);
				ofSetRectMode(OF_RECTMODE_CENTER);
				ofFill();
				ofPushMatrix();
				//ofTranslate(center.x , center.y);

				ofEllipse(center.x,center.y,10,10);
				string msg = ofToString(label) + ":" + ofToString(tracker.getAge(label));
				ofDrawBitmapString(msg,center.x,center.y);
				ofVec2f velocity = toOf(contourFinder[kinectNumber].getVelocity(i));

				ofPushMatrix();
				ofTranslate(center.x, center.y);
				ofScale(10, 10);
				ofLine(0, 0, velocity.x, velocity.y);
				ofPopMatrix();
				ofScale(sensorArea[kinectNumber],sensorArea[kinectNumber]);

				ofPopMatrix();
				ofPopStyle();
			}
		}
	}

	// draw instructions
	ofPushStyle();
	ofSetColor(255, 255, 255);
	stringstream reportStream;

	reportStream << ofGetFrameRate();
	ofDrawBitmapString(reportStream.str(), 20, 652);
	ofPopStyle();
	gui.draw();



}


//--------------------------------------------------------------

void ofApp::screenSetup(){

	syphonServer.setName("kinectTracking");

	ofSetWindowShape(CWIDTH,CHEIGHT); //set windowSize the same as the


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

	for(int kinectNumber = 0; kinectNumber < 2; kinectNumber++){
		kinect[kinectNumber].update();
		// there is a new frame and we are connected
		if(kinect[kinectNumber].isFrameNew()) {

			// load grayscale depth image from the kinect source
			grayImage[kinectNumber].setFromPixels(kinect[kinectNumber].getDepthPixels(), kinect[kinectNumber].width, kinect[kinectNumber].height);

			// we do two thresholds - one for the far plane and one for the near plane
			// we then do a cvAnd to get the pixels which are a union of the two thresholds
			grayThreshNear[kinectNumber] = grayImage[kinectNumber];
			grayThreshFar[kinectNumber] = grayImage[kinectNumber];
			grayThreshNear[kinectNumber].threshold(nearThreshold, true);
			grayThreshFar[kinectNumber].threshold(farThreshold[kinectNumber]);
			cvAnd(grayThreshNear[kinectNumber].getCvImage(), grayThreshFar[kinectNumber].getCvImage(), grayImage[kinectNumber].getCvImage(), NULL);


			// update the cv images
			grayImage[kinectNumber].flagImageChanged();

			contourFinder[kinectNumber].setMinAreaRadius(minBlobSize[kinectNumber]);
			contourFinder[kinectNumber].setMaxAreaRadius(maxBlobSize[kinectNumber]);
			contourFinder[kinectNumber].findContours(grayImage[kinectNumber]);
		}
	}
}

void ofApp::kinectSetup(int kinectNumber, string id){

	//A00367813858042A

	// enable depth.video image calibration
	kinect[kinectNumber].setRegistration(true);

	kinect[kinectNumber].init(false,false);

	//  kinect[kinectNumber].
	if(id.empty()){
		kinect[kinectNumber].open();
	}else{
		kinect[kinectNumber].open(id);
	}

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
//	kinect[kinectNumber].setCameraTiltAngle(10);

	nearThreshold = 255;

	//blob tracking system parameter
	contourFinder[kinectNumber].getTracker().setPersistence(10);
	contourFinder[kinectNumber].getTracker().setMaximumDistance(128);

}



void ofApp::guiSetup(){

	gui.setup("Settings", "settings.xml");

	for(int kinectNumber = 0; kinectNumber < 2; kinectNumber++){

		parametersKinect[kinectNumber].setName("Kinect " + ofToString(kinectNumber));
		parametersKinect[kinectNumber].add(farThreshold[kinectNumber].set("Far Threshold", 0,0, 255 ));
		parametersKinect[kinectNumber].add(numMaxBlobs[kinectNumber].set("Num Max Blos",10,0,15));
		parametersKinect[kinectNumber].add(maxBlobSize[kinectNumber].set("max Blob Size",0,0,500));
		parametersKinect[kinectNumber].add(minBlobSize[kinectNumber].set("min Blob Size",0,0,500));
		parametersKinect[kinectNumber].add(offsetX[kinectNumber].set("Offset X", 0,-200, 200 ));
		parametersKinect[kinectNumber].add(offsetY[kinectNumber].set("Offset Y", 0,-200, 200 ));
		parametersKinect[kinectNumber].add(sensorPos[kinectNumber].set("Sensor Pos", ofVec2f(1,10.0),ofVec2f(0,0),ofVec2f(CWIDTH,CHEIGHT)));
		parametersKinect[kinectNumber].add(sensorArea[kinectNumber].set("Sensor Area", 0.5, 0.1,2));
        parametersKinect[kinectNumber].add(waveDecayTime[kinectNumber].set("WaveDecayTime",1000,200,3000));
        parametersKinect[kinectNumber].add(waveColor[kinectNumber].set("WaveColor", ofColor(255,255), ofColor(0,0),ofColor(255,255)));
        gui.add(parametersKinect[kinectNumber]);
        
	}
	// events for change in paramenters on ofpp application
	gui.add(myBack.particlesGUI);
    
    gui.add(velocityRatio.set("Velo Ratio",1,0.1,2));
    gui.add(pObjDecTime.set("Physical Obj dTime",1000,1000,10000));

    gui.minimizeAll();

	gui.loadFromFile("settings.xml");

}
//Gui events for kinect Area


ofPoint ofApp::toWorldCoord(ofPoint point, int kinectId){

	//mapping position to a new area
	float x = ofMap(point.x, 0, kinect[kinectId].width, 0, kinect[kinectId].width * sensorArea[kinectId]) -  kinect[kinectId].width * sensorArea[kinectId]*0.5;
	float y = ofMap(point.y, 0, kinect[kinectId].height,kinect[kinectId].height * sensorArea[kinectId],0) -  kinect[kinectId].height * sensorArea[kinectId]*0.5;

	return ofPoint(y + sensorPos[kinectId]->x, x + sensorPos[kinectId]->y );

}

void ofApp::setRunningMode(canvasMode _newRunningMode) {
    appConfig.runningMode = _newRunningMode;
}

void ofApp::oscUpdate(){

	while(receiver.hasWaitingMessages()){
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);

		// check for mouse moved message
		if(m.getAddress() == "/mouse/position"){
			// both the arguments are int32's
			mouseX = m.getArgAsInt32(0);
			mouseY = m.getArgAsInt32(1);
		}


	}


}
//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
	switch (key) {

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
            for(int i = 0; i<8; i++){
                post[i]->setEnabled(false);
            }
			break;

		case 'm':
			break;

		case OF_KEY_UP:
			break;

		case OF_KEY_DOWN:
			break;

		case OF_KEY_LEFT:
			break;

		case OF_KEY_RIGHT:
			break;

		case 'z':
            appConfig.runningMode = TRAILS;
			break;
		case 'x':
            appConfig.runningMode = GRAPH;
			break;
        case 'c':
            appConfig.runningMode = SHAPES;
            break;
        case 'v':
            appConfig.runningMode = SQUARES;
            break;
        case '1':
            post[0]->setEnabled(!post[0]->getEnabled());
            break;
        case '2':
            post[1]->setEnabled(!post[1]->getEnabled());
            break;
        case '3':
            post[2]->setEnabled(!post[2]->getEnabled());
            break;
        case '4':
            post[3]->setEnabled(!post[3]->getEnabled());
            break;
        case '5':
            post[4]->setEnabled(!post[4]->getEnabled());
            break;
        case '6':
            post[5]->setEnabled(!post[5]->getEnabled());
            break;
        case '7':
            post[6]->setEnabled(!post[6]->getEnabled());
            break;
        case '8':
            post[7]->setEnabled(!post[7]->getEnabled());
            break;
            
        case '0':
            for(int i = 0; i<8; i++){
                post[i]->setEnabled(false);
            }
            break;
            
	}


}


void ofApp::mouseMoved(int x, int y){}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}

void ofApp::exit() {
	kinect[0].close();
	kinect[1].close();
}
