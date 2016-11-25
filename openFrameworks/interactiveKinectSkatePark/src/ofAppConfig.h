//
//  ofConfig.h
//  kinectSkate
//
//  Created by Edgar Zanella on 1/8/15.
//
//

#pragma once

enum canvasMode {
    TRAILS = 0,
    CREATE_OBJECTS,
    SQUARES,
    GRAPH,
    SHAPES

};

class AppConfig {
public:
    canvasMode runningMode;
    bool graphEnable = false;
    bool onlyParticles = false;
    int imageNameId = 0;

};


