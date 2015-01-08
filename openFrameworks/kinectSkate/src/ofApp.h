#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxSyphon.h"
#include "ofxGui.h"
#include "ofBackEffects.h"
#include "ofxOsc.h"
#include "ofWave.h"
#include "ofPhysicalObject.h"
#include "ofxTwistedRibbon.h"
#include "ofAppConfig.h"
#include "ofxPostProcessing.h"
#include <tr1/unordered_map>

#define HOST "localhost"
#define PORT 12345

class ContactData {
public:
    float x;
    float y;
    float r;
};

class ObjectData {
public:
	float w;
	float h;
	bool hit;
};

class ofApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
    void drawTrail();

	void exit();
	void drawPointCloud();
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void mouseMoved(int x, int y);

	void screenSetup();
	void kinectSetup(int kinectNumber, string id);
	void kinectUpdate();
	void debugMode();

	void updateTrail();
	void createObjects();
	// this is the function for contacts
	void contactStart(ofxBox2dContactArgs &e);
	void contactEnd(ofxBox2dContactArgs &e);
	void guiSetup();
	void drawPositions();
    void setRunningMode(canvasMode _newRunningMode);
    
	void oscUpdate();

	// convert blob position to world
	ofPoint toWorldCoord(ofPoint point, int kinectId);

	ofxSyphonServer syphonServer;

	//Kinects
	ofxKinect kinect[2];
	ofxCv::ContourFinder contourFinder[2];
	ofxCvColorImage colorImg[2];
	ofxCvGrayscaleImage grayImage[2]; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear[2]; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar[2]; // the far thresholded image

	//GUI
	ofxPanel gui; //

	ofParameterGroup parametersKinect[2];

	ofParameter<int> farThreshold[2];
	ofParameter<float> offsetX[2];
	ofParameter<float> offsetY[2];
	ofParameter<int> numMaxBlobs[2];
	ofParameter<int> minBlobSize[2];
	ofParameter<int> maxBlobSize[2];

	ofParameter<ofVec2f> sensorPos[2];
	ofParameter<float> sensorArea[2];

	vector <tr1::unordered_map<int, int> > addedObjs;

	//options variables
	bool bDebugMode = true;
	int nearThreshold;

	static const int CWIDTH = 1024*2; //canvas width 1
	static const int CHEIGHT = 768; //canvas height

	//ofscreen buffers
	ofFbo screen1;

	// Box2D
	ofxBox2d box2d;
	vector <ofPtr<ofxBox2dRect> > boxes;
	vector <ofPhysicalObject> physObjects;

	vector <vector <ofPoint> > trail;
	vector <int> trail_i;
	vector <int> addedTrailSegments;
	vector <ContactData> colCenters;

	ofBackEffects myBack;
	ofImage debugImage;

	// Waves
	vector <ofWave> waves;
    
	//oscMessage Sender
	ofxOscSender sender;
	ofxOscReceiver receiver;

    bool effect;
	long lastTime;
    ofImage objectImage;

    AppConfig appConfig;
    
    ofxPostProcessing post;
};
