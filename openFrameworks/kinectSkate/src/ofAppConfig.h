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
    GRAPH,
    SHAPES
};

class AppConfig {
public:
    canvasMode runningMode;
};


