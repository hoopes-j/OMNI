//
//  BinauralDisplay.hpp
//  spatializer
//
//  Created by Jason Hoopes on 4/9/24.
//

#pragma once

#include <stdio.h>
#include "ofMain.h"

class BinauralDisplayState {
public:
    float azimuth;
    float elevation;
    
    ofVec2f boxSize;
    ofVec2f boxPos;
    ofVec2f container;
};

class BinauralDisplay {
public:
    void setup(int numSpatializers);
    void update();
    void updatePosition(int idx, float azimuth, float elevation);
    void draw();
    void cleanup();
    
    std::vector<BinauralDisplayState> states;
    
    ofBoxPrimitive box;
    
    std::vector<ofVec3f> times;
    std::vector<ofVec3f> angles;
    ofEasyCam cam;

private:
    
    ofVec3f pos;
    int _numSpatializers;
};

