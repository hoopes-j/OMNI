//
//  GrainSliders.hpp
//  granular
//
//  Created by Jason Hoopes on 3/19/24.
//

#pragma once

#include <stdio.h>
#include "ofxGui.h"

class GrainSliders {
public:
    void setup(int numGrains);
    
    ofParameterGroup params;
    std::vector<ofParameter<float>> pitches;
    ofParameter<float> pitchMultiplier;
    
    
};



