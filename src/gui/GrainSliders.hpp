//
//  GrainSliders.hpp
//  granular
//
//  Created by Jason Hoopes on 3/19/24.
//

#pragma once

#include <stdio.h>
#include "ofxGui.h"
#include "ofxDropdown.h"

class GrainSliders {
public:
    void setup(int numGrains);
    void setPitches(float * newPitches);
    
    int numGrains;
    
    ofParameterGroup grainGroup;
    ofParameterGroup grainStarts;
    std::vector<ofParameter<float>> start;
    ofParameterGroup grainAmpsGroup;
    std::vector<ofParameter<float>> amps;
    
    ofParameterGroup params;
    std::vector<ofParameter<float>> pitches;
    ofParameter<float> pitchMultiplier;
    ofParameter<bool> manualPitch;
    ofParameter<float> pitchSpread;
    
    ofParameterGroup warpGroup;
    ofParameter<int> numWarpPoints;
    ofParameter<float> warpAmount;
    ofParameter<float> transientThreshold;
    
    
    unique_ptr<ofxDropdown> windowDropdown;
    bool setupWindowTypeSelector();
    void ofWindowTypeChanged(string & output);
    std::string currentWindowType;
    
    
};



