//
//  WaveformDisplay.hpp
//  granular
//
//  Created by Jason Hoopes on 4/17/24.
//

#pragma once

#include <stdio.h>
#include "ofMain.h"

struct WaveformDisplayConfig {
    int plotHeight = 20;
    int plotWidth = 500;
    int plotDepth = 100;
    
    int boxX = 0;
    int boxWidth = 200;
    int boxDepth = 100;
    
    int ptrWidth = 10;
    int ptrX = 0;
    
    int numBoxes = 1;
};



class WaveformDisplay {
    
public:
    bool setup(int xDimension, int yDimension);
    void draw();
    void update(const float * data, int length, int ptr, int grainLength, int grainStart);

private:
    int _xDimension;
    int _yDimension;
    
    std::vector<std::vector<float>> _gridData;
    ofMesh mesh;
    ofBoxPrimitive box;
    std::vector<ofBoxPrimitive> _boxes;
    std::vector<ofBoxPrimitive> _pointers;

    WaveformDisplayConfig _config;


};


