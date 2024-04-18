//
//  WaveformDisplay.hpp
//  granular
//
//  Created by Jason Hoopes on 4/17/24.
//

#pragma once

#include <stdio.h>
#include "ofMain.h"

class WaveformDisplay {
    
public:
    void setup(int xDimension, int yDimension);
    void draw();
    void update();
    

private:
    std::vector<std::vector<float>> gridData;

    int _xDimension;
    int _yDimension;
};


