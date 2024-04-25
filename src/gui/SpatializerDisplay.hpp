//
//  SpatializerDisplay.hpp
//  spatializer
//
//  Created by Jason Hoopes on 4/9/24.
//

#ifndef SpatializerDisplay_hpp
#define SpatializerDisplay_hpp

#include <stdio.h>
#include "ofMain.h"

struct SpatializerDisplayState {
    float azimuth;
    float elevation;
    
    ofVec2f boxSize;
    ofVec2f boxPos;
    ofVec2f container;
};

class SpatializerDisplay {
public:
    void setup();
    void update();
    void updatePosition(float azimuth, float elevation);
    void draw();
    void cleanup();
    
    SpatializerDisplayState state;
    
    ofBoxPrimitive box;
    
    std::vector<ofVec3f> times;
    std::vector<ofVec3f> angles;
    ofEasyCam cam;

private:
};

#endif /* SpatializerDisplay_hpp */
