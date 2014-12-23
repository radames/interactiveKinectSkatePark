#include "ofApp.h"
#include "ofxSyphon.h"

using namespace cv;
using namespace ofxCv;

//--------------------------------------------------------------
void ofApp::setup() {

    screenSetup(); //screen and some OF setups
    kinectSetup(0,"A00367813858042A"); //kinetic setup
    kinectSetup(1,""); //kinetic setup


    guiSetup(); //GUI Setup


}

//--------------------------------------------------------------
void ofApp::update() {

    ofEnableAlphaBlending();
	ofBackground(255, 255, 255);
    kinectUpdate();

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
    const vector<unsigned int>& newLabels = tracker.getNewLabels();
    const vector<unsigned int>& deadLabels = tracker.getDeadLabels();


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

    if(bDebugMode){ debugMode(); }//draw debug mode

    // cleaning alls screens
    screen1.begin();
    ofClear(255,255,255, 0);
    screen1.end();
    
    
    screen1.draw(0,0);
    
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
