#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxSyphon.h"
#include "ofxGui.h"


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
    void kinectSetup();
    void kinectUpdate();
    void debugMode();
    

    void guiSetup();
    
    //gui Change events
    void kinectUpdateAreaW(int& kinectWidth);
    void kinectUpdateAreaH(int& kinectHeight);
    
    
    // blob Offset detais
    float applyOffsetX(float _x);
    float applyOffsetY(float _y);


    
	ofxKinect kinect1,kinect2;
    ofxSyphonServer syphonServer;
    
    ofxCv::ContourFinder contourFinder;


	ofxCvColorImage colorImg1;
	ofxCvGrayscaleImage grayImage1; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear1; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar1; // the far thresholded image
   
    ofxCvColorImage colorImg2;
    ofxCvGrayscaleImage grayImage2; // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear2; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar2; // the far thresholded image
    
    
    //GUI
    ofxPanel gui; //

    ofParameterGroup parametersKinect;
    

    ofParameter<bool> enableMouse;
    ofParameter<int> farThreshold = 143;
    ofParameter<float> offsetX;
    ofParameter<float> offsetY;
    ofParameter<int> numMaxBlobs = 15;
    ofParameter<int> minBlobSize = 22;
    ofParameter<int> maxBlobSize = 354;
    ofParameter<int> kinectWidth = 640;
    ofParameter<int> kinectHeight = 480;
    
    
    //options variables
    bool bDebugMode = true;
	int nearThreshold;
	
    float blobx,bloby;
        
    static const int CHEIGHT = 1980; //canvas height
    static const int CWIDTH = 1280; //canvas width 1

    
    //ofscreen buffers
    ofFbo screen1;

};
