//
//  SpatialSliders.hpp
//  granular
//
//  Created by Jason Hoopes on 4/24/24.
//

#pragma once

#include <stdio.h>
#include "ofxGui.h"


class SpatialSliders {
public:
    void setup(int num, std::string type);
    
    int numSpatializers;
    std::string spatializerType;
    ofParameterGroup params;
    std::vector<ofParameterGroup> spatializerGroups;
    // Binaural Sliders
    std::vector<ofParameter<int>> azimuths;
    std::vector<ofParameter<int>> elevations;
    
    // Loudspeaker sliders
    std::vector<ofParameter<float>> positions;
};
