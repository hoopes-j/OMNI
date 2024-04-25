//
//  WaveformDisplay.cpp
//  granular
//
//  Created by Jason Hoopes on 4/17/24.
//

#include "WaveformDisplay.hpp"


   
bool WaveformDisplay::setup(int xDimension, int yDimension)
{
    //    glDisable(GL_LINE_SMOOTH);
    //    glDisable(GL_MULTISAMPLE);
    
    _xDimension = xDimension;
    _yDimension = yDimension;
    
    
    _gridData.resize(yDimension);
    for (int i = 0; i < yDimension; i++) {
        _gridData[i].resize(xDimension);
    }
    
    mesh.setMode(OF_PRIMITIVE_LINES);
    
    ofVec3f spacing = ofVec3f(
                              _config.plotWidth/xDimension,
                              _config.plotHeight,
                              _config.plotDepth/(yDimension-1)
                              );
    
    ofVec3f pos = ofVec3f(-250,0,0);
    
    for (int z = 0; z < yDimension; z++) {
        for (int x = 0; x < xDimension; x++) {
            float h = ofRandomf()/2;
            ofVec3f vertex(x, h, z);
            mesh.addVertex(vertex*spacing+pos);
            mesh.addColor(ofFloatColor(1-(h+0.5)));
        }
    }
    
    for (int y = 0; y < yDimension; y++) {
        for (int x = 0; x < xDimension; x++) {
            
            int rn = y*xDimension+x;
            
            int u = rn+xDimension;
            int r = rn+1;
            int ur = u+1;
            
            if (x<xDimension-1){
                mesh.addIndex(rn);
                mesh.addIndex(r);
                //                mesh.addIndex(u);
                
                //                mesh.addIndex(u);
                //                mesh.addIndex(r);
                //                mesh.addIndex(ur);
            }
            
        }
    }
    
    
    _boxes.resize(_config.numBoxes);
    _pointers.resize(_config.numBoxes);
    for (int i = 0; i < _config.numBoxes; i++) {
        _boxes[i].set(
                _config.boxWidth,
                _config.plotHeight,
                _config.boxDepth
        );
        _pointers[i].set(
                _config.ptrWidth,
                _config.plotHeight,
                _config.boxDepth
        );

        int j = 0;
        while(j<6) {
            _boxes[i].setSideColor(j, ofFloatColor(1,1,0,0.25));
            _pointers[i].setSideColor(j, ofFloatColor(0,1,1,0.25));
            j++;
        }
    }


    return true;
}

void WaveformDisplay::draw()
{
    
    

    ofSetLineWidth(20);
    ofNoFill();
    mesh.draw();
    
    for (int i = 0; i < _config.numBoxes; i++) {
        _boxes[i].setPosition(_config.boxX-_config.plotWidth/2, 0, _config.plotDepth/2);
        _boxes[i].setWidth(_config.boxWidth);
        
        _pointers[i].setPosition(_config.ptrX-(_config.plotWidth/2+20), 0, _config.plotDepth/2);
        
        int j = 0;
        while(j<6) {
            _boxes[i].setSideColor(j, ofFloatColor(1,1,0,0.25));
            j++;
        }

        _boxes[i].draw();
        _pointers[i].draw();
    }
    
}

void WaveformDisplay::update(const float * data, int length, int ptr, int grainLength, int grainStart) {
    
    _config.ptrX = floor((float)ptr/length*_config.plotWidth);
    _config.boxWidth = floor((float)grainLength/length*_config.plotWidth);
    _config.boxX = floor((float)grainStart/length*_config.plotWidth);
    
    
    int stepSize = length/_xDimension;
    for (int x = 0; x < _xDimension; x++) {
        for (int z = 0; z < _yDimension; z++) {
            int sampleIdx = stepSize*x;
            ofVec3f oldV = mesh.getVertex(x+z*_xDimension);
            float newY = abs(data[sampleIdx])*_config.plotHeight*5;
            mesh.setColor(x+z*_xDimension,ofFloatColor(1-(newY/_config.plotHeight)));
            ofVec3f newV = ofVec3f(oldV.x,newY,oldV.z);
            mesh.setVertex(x+z*_xDimension,newV);
        }

    }
}
