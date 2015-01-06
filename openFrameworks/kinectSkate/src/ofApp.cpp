#include "ofApp.h"
#include "ofxBox2d.h"
#include "ofxSyphon.h"

using namespace cv;
using namespace ofxCv;

//--------------------------------------------------------------
void ofApp::setup() {
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

void ofApp::drawWave() {

	long now = ofGetElapsedTimeMillis();
	long dt = now - waveTime;

	if (waveTime != -1) {
		ofPushStyle();
		//ofNoFill();
		ofSetLineWidth(10);
		//ofSetCircleResolution(100);
		//setStrokeColor(255,0,0);
		ofPath circle;
		circle.setCircleResolution(500);
		circle.arc(startWave, dt*0.1, dt*0.1, 0, 360);
		circle.close();
		circle.arc(startWave, dt*0.1 + 40, dt*0.1 + 40, 0, 360);
		//circle.setStrokeColor(ofColor(255,0,0));
		//circle.setStrokeWidth(100);
		circle.draw();

		//ofCircle(startWave.x, startWave.y, 10 + dt*0.1);
		ofPopStyle();

		if (dt > 5000) {
			waveTime = -1;
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
			// cout << "NEW POS " << newPosition << endl;
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
				float w = 20;
				float h = 20;
				ofPoint center = toWorldCoord(toOf(contourFinder[kinectNumber].getCenter(i)), kinectNumber);

				/*
				   ofVec3f position;
				   position.z = 0;
				   position.x  = center.x;
				   position.y  = center.y;
				   ofColor color;
				   int hue = int(ofGetElapsedTimef() * 10) % 255;
				   ofPtr<ofxTwistedRibbon> newRibbon;
				   ribbons[j][label] = newRibbon;
				   newRibbon->update(position, color); */

				// Create new wave
				ofPtr<ofWave> newWave = ofPtr<ofWave>(new ofWave);
				waves.push_back(newWave);
				newWave->setup(center, ofGetElapsedTimeMillis(), 10, 50, (kinectNumber==0?123:255));

				// Add Physical Object
				ofVec2f velocity = toOf(tracker.getVelocity(i));
				ofPtr<ofPhysicalObject> ptrPhysicalObject = ofPtr<ofPhysicalObject>(new ofPhysicalObject);
				ofPhysicalObject *physicalObject = ptrPhysicalObject.get();
				physicalObject->setup(&box2d, velocity, center, kinectNumber, label, w, h);
				physObjects.push_back(ptrPhysicalObject);

				// Add trail to the new object

				/*
				   vector <ofPoint> box_trail;
				   box_trail.assign(30, ofPoint());
				   for (int ti = 0; ti < 30; ++ti) {
				   box_trail[ti] = center;
				   }
				   trail.push_back(box_trail);

				   ofxBox2dRect *rect = box.get();
				   ofVec2f velocity = toOf(tracker.getVelocity(i));

				   rect->setVelocity(velocity.x, velocity.y);
				   rect->setPhysics(3.0, 0.53, 0.1);
				   rect->setup(box2d.getWorld(), center.x, center.y, w, h);
				 */
				// Add attract points to background
				myBack.addAttractPoints(center);

				/*
				   rect->setData(new ObjectData());
				   ObjectData *objData = (ObjectData *)rect->getData();
				   objData->w = velocity.x * w;
				   objData->h = velocity.y * h;
				   objData->hit = true;*/

				addedObjs[kinectNumber][label] = boxes.size() - 1;

				//Osc Message for new Objects on the screen based on the sensorPositions[j] j = kinectic number
				ofxOscMessage m;

				if(center.x  < sensorPos[kinectNumber]-> x ){ //if this then he've appeared first on th left
					m.setAddress("/skatista/ED");
				}else{
					m.setAddress("/skatista/DE");
				}
				m.addIntArg(kinectNumber); // which sensor plus velocity
				m.addFloatArg(velocity.x);
				m.addFloatArg(velocity.y);
				sender.sendMessage(m);

			}

			if (tracker.existsPrevious(label) && addedObjs[kinectNumber][label] != -1) {

				ofVec2f velocity = toOf(tracker.getVelocity(i));
				ofPtr<ofxBox2dRect> rect = boxes[addedObjs[kinectNumber][label]];

				if (velocity.x != 0 && velocity.y != 0) {

					rect->setData(new ObjectData());
					ObjectData *objData = (ObjectData *)rect->getData();
					objData->w = velocity.x*20;
					objData->h = velocity.y *20;
					objData->hit = true;

					rect->setVelocity(-velocity.y, velocity.x);
					addedObjs[kinectNumber][label] == -1;
				}
			}
		}
		//loop nas kinects
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

	/*
	   if (boxes.size() > 0) {
	   ofVec3f position;
	   ofPoint newPosition = boxes[0].get()->getPosition();

	   float radius = sin(ofGetElapsedTimef()) * 50 + 200;
	   position.z = sin(ofGetElapsedTimef() * 0.3) * radius;
//ribbonZ = position.z;
position.x  = newPosition.x;
position.y  = newPosition.y;
ofColor color;
int hue = int(ofGetElapsedTimef() * 10) % 255;
color.setHsb(255, 120, hue);
ribbon->update(position, color);
}*/

// Update waves
for (int i = 0; i < waves.size(); ++i) {
	waves[i]->update();
}

}

//--------------------------------------------------------------
void ofApp::draw() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ofEnableAlphaBlending();

	/*
	   if (boxes.size() > 0) {
	   ofPoint newPosition = boxes[0].get()->getPosition();
	   ofPushMatrix();
	//ofTranslate(newPosition.x, newPosition.y, ribbonZ);
	//ofRotate(ofGetElapsedTimef() * 10, 1, 0, 0);
	ribbon->draw();
	ofPopMatrix();
	}*/

	if(bDebugMode){ debugMode(); }//draw debug mode

	for (int i = 0; i < physObjects.size(); ++i) {
		physObjects[i].get()->draw();
	}

	/*
	   for(int i=0; i<boxes.size(); i++) {
	   ofFill();
	   ofSetHexColor(0xe63b8b);
	   boxes[i].get()->draw();
// cout << boxes[i].get()->getPosition() << endl;
}*/

// drawPositions();
myBack.draw(); //draw background effects

// draw objects trail
drawTrail();
// draw waves
for (int i = 0; i < waves.size(); ++i) {
	cout << "DRAW "<<endl;
	waves[i]->draw();
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

	for(int j = 0; j < 2; j++){
		//drawing two depth areas

		ofPushMatrix();

		ofPushStyle();
		ofSetRectMode(OF_RECTMODE_CENTER);

		ofTranslate(sensorPos[j]->x, sensorPos[j]->y);
		ofScale(sensorArea[j],sensorArea[j]);
		ofRotate(90);
		kinect[j].drawDepth(0,0,kinect[j].width, kinect[j].height);
		ofPushMatrix();
		ofTranslate(-kinect[j].width/2,-kinect[j].height/2);
		ofSetRectMode(OF_RECTMODE_CORNER);
		contourFinder[j].draw();
		ofPopMatrix();
		ofPopStyle();
		ofPopMatrix();



		RectTracker& tracker = contourFinder[j].getTracker();

		for(int i=0; i < contourFinder[j].size(); i++){

			unsigned int label = contourFinder[j].getLabel(i);

			if(tracker.existsPrevious(label)) {

				ofPoint center = toWorldCoord( toOf(contourFinder[j].getCenter(i)),j);
				ofPushStyle();
				ofSetColor(255,0,0);
				ofSetRectMode(OF_RECTMODE_CENTER);
				ofFill();
				ofPushMatrix();
				//ofTranslate(center.x , center.y);

				ofEllipse(center.x,center.y,10,10);
				string msg = ofToString(label) + ":" + ofToString(tracker.getAge(label));
				ofDrawBitmapString(msg,center.x,center.y);
				ofVec2f velocity = toOf(contourFinder[j].getVelocity(i));

				ofPushMatrix();
				ofTranslate(center.x, center.y);
				ofScale(10, 10);
				ofLine(0, 0, velocity.x, velocity.y);
				ofPopMatrix();
				ofScale(sensorArea[j],sensorArea[j]);

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
	kinect[kinectNumber].setCameraTiltAngle(10);

	nearThreshold = 255;

	//blob tracking system parameter
	contourFinder[kinectNumber].getTracker().setPersistence(10);
	contourFinder[kinectNumber].getTracker().setMaximumDistance(128);

}



void ofApp::guiSetup(){

	gui.setup("Settings", "settings.xml");

	for(int i = 0; i < 2; i++){

		parametersKinect[i].setName("Kinect " + ofToString(i));
		parametersKinect[i].add(farThreshold[i].set("Far Threshold", 0,0, 255 ));
		parametersKinect[i].add(numMaxBlobs[i].set("Num Max Blos",10,0,15));
		parametersKinect[i].add(maxBlobSize[i].set("max Blob Size",0,0,500));
		parametersKinect[i].add(minBlobSize[i].set("min Blob Size",0,0,500));
		parametersKinect[i].add(offsetX[i].set("Offset X", 0,-200, 200 ));
		parametersKinect[i].add(offsetY[i].set("Offset Y", 0,-200, 200 ));
		parametersKinect[i].add(sensorPos[i].set("Sensor Pos", ofVec2f(1,10.0),ofVec2f(0,0),ofVec2f(CWIDTH,CHEIGHT)));
		parametersKinect[i].add(sensorArea[i].set("Sensor Area", 0.5, 0.1,2));

		gui.add(parametersKinect[i]);
	}
	// events for change in paramenters on ofpp application
	gui.add(myBack.particlesGUI);

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
			break;
		case 'x':

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
