#include "ofApp.h"
#include "ofxSyphon.h"

using namespace cv;
using namespace ofxCv;

//--------------------------------------------------------------
void ofApp::setup() {

    screenSetup(); //screen and some OF setups
    kinectSetup(); //kinetic setup
    guiSetup(); //GUI Setup


}

//--------------------------------------------------------------
void ofApp::update() {

    ofEnableAlphaBlending();
	ofBackground(255, 255, 255);
    kinectUpdate();

    //varre os blobs, checa
    RectTracker& tracker = contourFinder.getTracker();
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
    ofPushStyle();

        // draw from the live kinect
        kinect1.draw(0, 200, 300, 200);
        kinect1.drawDepth(0, 0, 300, 200);
    
        kinect2.draw(300, 200, 300, 200);
        kinect2.drawDepth(300, 0, 300, 200);
    
    ofPopStyle();

    ofPushMatrix();

        ofTranslate(0,400);
        ofScale(300.0/kinect1.width,200.0/kinect1.height);

        grayImage1.draw(0,0);

            ofPushMatrix();
                ofTranslate(kinect1.width/2,kinect1.height/2);
                ofPushStyle();
                //drawing the actual kinect area, used to the positions calculation inside de morphrender funciton
                ofSetRectMode(OF_RECTMODE_CENTER);
                ofSetColor(255,0,0,100);
                ofFill();
                ofRect(0,0, kinectWidth, kinectHeight);
                ofPopStyle();
            ofPopMatrix();
        contourFinder.draw();

    ofPushStyle();
    //draw simuled blob
    ofSetColor(0,255,0);
    ofFill();
    ofCircle(blobx ,bloby, 5);

    ofPopStyle();

    ofPopMatrix();

    //loop through all blobs detected and draw the centroid and lables

    RectTracker& tracker = contourFinder.getTracker();
    
    for(int i=0; i < contourFinder.size(); i++){
        unsigned int label = contourFinder.getLabel(i);

        if(tracker.existsPrevious(label)) {

            ofPoint center = toOf(contourFinder.getCenter(i));
            ofPushStyle();
            ofSetColor(255,0,0);
            ofFill();
            ofPushMatrix();
                ofTranslate(0,400);
                //ofScale(300.0/kinect.width,200.0/kinect.height);
                ofEllipse(center.x,center.y,10,10);
                string msg = ofToString(label) + ":" + ofToString(tracker.getAge(label));
                ofDrawBitmapString(msg,center.x,center.y);
                ofVec2f velocity = toOf(contourFinder.getVelocity(i));
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

    kinect1.update();
    kinect2.update();

    // there is a new frame and we are connected
    if(kinect1.isFrameNew()) {

        // load grayscale depth image from the kinect source
        grayImage1.setFromPixels(kinect1.getDepthPixels(), kinect1.width, kinect1.height);

        // we do two thresholds - one for the far plane and one for the near plane
        // we then do a cvAnd to get the pixels which are a union of the two thresholds
        grayThreshNear1 = grayImage1;
        grayThreshFar1 = grayImage1;
        grayThreshNear1.threshold(nearThreshold, true);
        grayThreshFar1.threshold(farThreshold);
        cvAnd(grayThreshNear1.getCvImage(), grayThreshFar1.getCvImage(), grayImage1.getCvImage(), NULL);


        // update the cv images
        grayImage1.flagImageChanged();

        contourFinder.setMinAreaRadius(minBlobSize);
        contourFinder.setMaxAreaRadius(maxBlobSize);
        contourFinder.findContours(grayImage1);

    }

}
void ofApp::kinectSetup(){

    // enable depth.video image calibration
    kinect1.setRegistration(true);

    kinect1.init();
    kinect2.init();

    //kinect.init(true); // shows infrared instead of RGB video image
    //kinect.init(false, false); // disable video image (faster fps)

    kinect1.open();
    kinect2.open();		// opens first available kinect
    //kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
    //kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #

    // print the intrinsic IR sensor values
    if(kinect1.isConnected()) {
        ofLogNotice() << "Kinect 1 Connected";
        ofLogNotice() << "sensor-emitter dist: " << kinect1.getSensorEmitterDistance() << "cm";
        ofLogNotice() << "sensor-camera dist:  " << kinect1.getSensorCameraDistance() << "cm";
        ofLogNotice() << "zero plane pixel size: " << kinect1.getZeroPlanePixelSize() << "mm";
        ofLogNotice() << "zero plane dist: " << kinect1.getZeroPlaneDistance() << "mm";
    }
    
    
    
    if(kinect2.isConnected()) {
        ofLogNotice() << "Kinect 1 Connected";
        ofLogNotice() << "sensor-emitter dist: " << kinect2.getSensorEmitterDistance() << "cm";
        ofLogNotice() << "sensor-camera dist:  " << kinect2.getSensorCameraDistance() << "cm";
        ofLogNotice() << "zero plane pixel size: " << kinect2.getZeroPlanePixelSize() << "mm";
        ofLogNotice() << "zero plane dist: " << kinect2.getZeroPlaneDistance() << "mm";
    }


    colorImg1.allocate(kinect1.width, kinect1.height);
    grayImage1.allocate(kinect1.width, kinect1.height);
    grayThreshNear1.allocate(kinect1.width, kinect1.height);
    grayThreshFar1.allocate(kinect1.width, kinect1.height);

    colorImg2.allocate(kinect2.width, kinect2.height);
    grayImage2.allocate(kinect2.width, kinect2.height);
    grayThreshNear2.allocate(kinect2.width, kinect2.height);
    grayThreshFar2.allocate(kinect2.width, kinect2.height);

    
    ofSetFrameRate(60);
    // zero the tilt on startup
    kinect1.setCameraTiltAngle(0);
    kinect2.setCameraTiltAngle(0);

    nearThreshold = 255;

    //blob tracking system parameter
    contourFinder.getTracker().setPersistence(10);
    contourFinder.getTracker().setMaximumDistance(128);
    
}



void ofApp::guiSetup(){

    gui.setup("Settings", "settings.xml", 310,100);

    gui.add(enableMouse.set("Mouse DEBUG",true));
    parametersKinect.setName("Kinect");
    parametersKinect.add(kinectWidth.set("Width",640, 540,740));
    parametersKinect.add(kinectHeight.set("Height",480, 380,580));



    parametersKinect.add(farThreshold.set("Far Threshold", 0,0, 255 ));
    parametersKinect.add(numMaxBlobs.set("Num Max Blos",10,0,15));
    parametersKinect.add(maxBlobSize.set("max Blob Size",0,0,500));
    parametersKinect.add(minBlobSize.set("min Blob Size",0,0,500));

    parametersKinect.add(offsetX.set("Offset X", 0,-200, 200 ));
    parametersKinect.add(offsetY.set("Offset Y", 0,-200, 200 ));



    gui.add(parametersKinect);

    gui.minimizeAll();
    // events for change in paramenters on ofpp application
    kinectWidth.addListener(this,&ofApp::kinectUpdateAreaW);
    kinectHeight.addListener(this,&ofApp::kinectUpdateAreaH);

    gui.loadFromFile("settings.xml");

}
//Gui events for kinect Area
void ofApp::kinectUpdateAreaW(int& kinectWidth){

}

void ofApp::kinectUpdateAreaH(int& kinectHeight){

}


float ofApp::applyOffsetX(float _x){
    return _x + offsetX;

}
float ofApp::applyOffsetY(float _y){
    return _y + offsetY;
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
    kinect1.close();
    kinect2.close();
}
