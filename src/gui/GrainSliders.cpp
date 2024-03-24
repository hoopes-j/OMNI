//
//  GrainSliders.cpp
//  granular
//
//  Created by Jason Hoopes on 3/19/24.
//

#include "GrainSliders.hpp"


void GrainSliders::setup(int numGrains) {
    params.setName("pitch");
    
    pitches.resize(numGrains);
    for (int i = 0; i < numGrains; i++) {
        std::string sliderName = "pitch"+std::to_string(i);
        params.add(pitches[i].set(sliderName, 1.0, -2.0, 2.0));
    }
    
    params.add(pitchMultiplier.set("global multiplier", 1.0, -2.0, 2.0));
}
