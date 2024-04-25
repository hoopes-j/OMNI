//
//  BufferDisplay.hpp
//  granular
//
//  Created by Jason Hoopes on 4/25/24.
//

#pragma once

#include <stdio.h>
#include "ofMain.h"

struct BufferDisplayConfig {
    
    int transientHeight = 50;
    int transientWidth = 3;
    int boxHeight = 100;
    
    int bufferHeight = 150;
    int bufferWidth = 1000;
    int containerRadius = 5;
    
    ofColor bgColor = ofColor(255,255,255, 255);
    ofColor bufferColor = ofColor(0,0,0, 100);
    ofColor pointerColor = ofColor(255,0,0,150);
    ofColor transientColor = ofColor(255,150,0,255);
    
    int ptrHeight = 100;
    
};


class BufferDisplay {
    
public:
    bool setup(ofVec2f pos,
               int numVoices);
    void draw();
    void update(const float * data, int length, int writePointer);
    void setWarpPoints(std::vector<int> warpPoints, bool draw);
    void setPointer(int idx, ofPolyline ptrLine);
    void drawGrainPointer(int idx, float ptr, float windowAmount, float windowVal, float amp);
   

private:
    ofPolyline drawBuffer(const float *buffer);

    
    ofVec2f _pos;
    
    int _numVoices;
    int _writePointer;
    int _bufferSize;
    
    ofPolyline _bufferPolyline;
    
    std::vector<int> _warpPoints;
    bool _drawWarpPoints;
    std::vector<ofPolyline> _ptrPolylines;
    

    BufferDisplayConfig _config;


};
