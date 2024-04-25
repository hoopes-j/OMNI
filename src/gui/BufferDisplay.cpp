//
//  BufferDisplay.cpp
//  granular
//
//  Created by Jason Hoopes on 4/25/24.
//

#include "BufferDisplay.hpp"



bool BufferDisplay::setup(ofVec2f pos, int numVoices) {
    _pos = pos;
    _numVoices = numVoices;
    _ptrPolylines.resize(_numVoices);
    return true;
}

void BufferDisplay::draw()
{

        ofSetLineWidth(1);
        ofFill();
        ofSetColor(_config.bgColor);
        ofDrawRectRounded(_pos.x, _pos.y, _config.bufferWidth, _config.bufferHeight, _config.containerRadius);
        ofSetColor(_config.bufferColor);
        _bufferPolyline.draw();
        ofSetLineWidth(3);
    
        ofSetColor(_config.pointerColor);
        for (int i = 0; i < _numVoices; i++) {
            _ptrPolylines[i].draw();
        }
    
    if (_drawWarpPoints) {
        ofSetColor(_config.transientColor);
        for (int i = 0; i < _warpPoints.size(); i++) {
            float idx = (float)_warpPoints[i]+(_bufferSize-_writePointer)%_bufferSize;
            float x = (idx/(float)_bufferSize)*_config.bufferWidth+_pos.x;
            ofDrawRectangle(x, _pos.y-_config.transientHeight/2+_config.bufferHeight/2, _config.transientWidth, _config.transientHeight);
        }
    }

}

void BufferDisplay::update(const float * data, int length, int writePointer) {
    this->_bufferSize = length;
    this->_writePointer = writePointer;
    _bufferPolyline = this->drawBuffer(data);
}

ofPolyline BufferDisplay::drawBuffer(const float *buffer) {
    
    ofPolyline bufferGraphic;
    bufferGraphic.clear();
    
    int div = 10;
    for(size_t i = 0; i < _bufferSize/div; i++) {
        int idx = (i*div+_writePointer)%_bufferSize;
        float sample = buffer[idx];
        float y = _pos.y-sample*_config.bufferHeight;
        float x = i*((float)div)/(float)_bufferSize*_config.bufferWidth+_pos.x;
        bufferGraphic.addVertex(x, y+_config.bufferHeight/2);
    }
    
        
    return bufferGraphic;
}

void BufferDisplay::setWarpPoints(std::vector<int> warpPoints, bool draw) {
    _warpPoints = warpPoints;
    _drawWarpPoints = draw;
}

void BufferDisplay::setPointer(int idx, ofPolyline ptrLine) {
    _ptrPolylines[idx] = ptrLine;
}

void BufferDisplay::drawGrainPointer(int idx, float ptr, float windowAmount, float windowVal, float amp) {
    
    ofPolyline pointerLine;
    pointerLine.clear();
    
        int rPos = floor(ptr);
        int wPos = _writePointer;
        float lineHeight = (_config.ptrHeight-_config.ptrHeight*((1-windowVal)*windowAmount))*amp;

        float yT = _pos.y-lineHeight/2+_config.bufferHeight/2;
        float yB = _pos.y+lineHeight/2+_config.bufferHeight/2;
    
        int pos = (_bufferSize-(wPos-rPos))%_bufferSize;
        float x = pos/(float)_bufferSize*_config.bufferWidth+_pos.x;
        

        pointerLine.addVertex(x,yT);
        pointerLine.addVertex(x, yB);
    
    _ptrPolylines[idx] = pointerLine;
}
