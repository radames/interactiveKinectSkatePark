#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxSyphon.h"
#include "ofxGui.h"
#include "ofBackEffects.h"

#include <tr1/unordered_map>

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
    
    void createObjects();
	// this is the function for contacts
	void contactStart(ofxBox2dContactArgs &e);
	void contactEnd(ofxBox2dContactArgs &e);
    
    void guiSetup();
    
    void drawPositions();
    
    //gui Change events
    void kinectUpdateAreaW(int& kinectWidth);
    void kinectUpdateAreaH(int& kinectHeight);
    
    
    // blob Offset detais
    float applyOffsetX(float _x);
    float applyOffsetY(float _y);

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

    ofParameter<bool> enableMouse;

    ofParameterGroup parametersKinect[2];

    ofParameter<int> farThreshold[2];
    ofParameter<float> offsetX[2];
    ofParameter<float> offsetY[2];
    ofParameter<int> numMaxBlobs[2];
    ofParameter<int> minBlobSize[2];
    ofParameter<int> maxBlobSize[2];

    tr1::unordered_map<int, int> addedObjs;

    
    //options variables
    bool bDebugMode = true;
	int nearThreshold;
	
    float blobx,bloby;
        
    static const int CHEIGHT = 1024*2; //canvas height
    static const int CWIDTH = 768; //canvas width 1

    
    //ofscreen buffers
    ofFbo screen1;
    
    // Box2D
    ofxBox2d box2d;
	vector <ofPtr<ofxBox2dRect> > boxes;
    vector <ContactData> colCenters;

    ofBackEffects myBack;
};
