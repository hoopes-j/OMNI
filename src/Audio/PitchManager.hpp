//
//  PitchManager.hpp
//  granular
//
//  Created by Jason Hoopes on 4/3/24.
//

#pragma once

#include <stdio.h>
#include <vector>

struct Scale {
    int len;
    std::vector<float> pitches;
};


class PitchManager {
public:
    
    bool setup() {
        
        int tuningSteps = 12;
        
        tuning.resize(tuningSteps);
        for (int i = 0; i < tuningSteps; i++) {
            tuning[i] = std::powf(2,i/12.f);
        }
        
        scale.len = 7;
        scale.pitches.resize(scale.len);
        for (int i = 0; i < scale.len; i++) {
            scale.pitches[i] = tuning[majorScale[i]];
        }
        
        pitchesNeedUpdate = true;
        
        spread=1.0;
        
        generate(8);
        
        
        
        return true;
    }
    
    
    /*
     TODO: better way to handle memory here?
     */
    std::vector<float> generate(int n) {
        std::vector<float> pitches(n);
        for (int i = 0; i < n; i++) {
            int m = floorf(spread*(float)n);
            int idx = std::rand()%m-1;
            
            pitches[i]=scale.pitches[idx];
        }

        return pitches;
    }
    
    std::vector<float> generate(int n, float s) {
        if (this->spread != s) {
            this->spread = s;
            pitches.clear();
            pitches.resize(n);
            for (int i = 0; i < n; i++) {
                int m = floorf(s*(float)n)+1;
                int idx = std::rand()%m;
                
                pitches[i]=this->scale.pitches[idx];
            }
            pitchesNeedUpdate=false;
        }
        
        return pitches;
    }
    
    
    
    
    
    
    
    
    
    std::vector<float> tuning;
    std::vector<float> pitches;
    bool pitchesNeedUpdate;
    
    Scale scale;
    
    
    float spread;
    float root;
    
    int majorScale[7] = {0,2,4,5,7,9,11};
};
