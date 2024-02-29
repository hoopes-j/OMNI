//
//  Granular.cpp
//  granular
//
//  Created by Jason Hoopes on 1/17/24.
//

#include "Granular.hpp"


GranularConfig::GranularConfig() {
    this->delayTime = NULL;
    this->sampleRate = NULL;
    this->feedback = NULL;
}

GranularConfig::~GranularConfig() {

}


bool Granular::setup(GranularConfig config) {
    this->_config = config;
    _delayBuffer = (float *) malloc(sizeof(float)*_config.delayBufferSize);
    _state.writePointer = 0;
    _state.readPointer = _config.delayBufferSize-_config.delayTime;
    _state.loopPointer = this->_config.loopStart;
    
    _particles.resize(_config.numGrains);
    for (int i = 0; i < _particles.size(); i++) {
        _particles[i].writePointer = 0;
        _particles[i].readPointer = _config.delayBufferSize-_config.delayTime;
        _particles[i].loopPointer = this->_config.loopStart;
    }
    
    return true;
}

bool Granular::updateConfig(GranularConfig config) {
    _config = config;
    _state.readPointer = (this->_config.delayBufferSize+(_state.writePointer-_config.delayTime))%this->_config.delayBufferSize;
    return true;
}




float Granular::processDelay(float input) {
//    float spray = ofRandomf()*10-5;
    float spray = 0;
    int readIndex = _state.readPointer+spray;
    
    float out = _delayBuffer[readIndex];
    _state.readPointer = (_state.readPointer + 1) % _config.delayBufferSize;
    /* if (!this->_config.loopOn) */ {
        this->updateBuffer(input+(out*this->_config.feedback));
    }
    return out;
}


void Granular::updateBuffer(float value) {
    _delayBuffer[_particles[0].writePointer] = value;
    for (int i = 0; i < _config.numGrains; i++) {
        // Should not have seperate write pointer per grain
        _particles[i].writePointer = (_particles[i].writePointer + 1) % _config.delayBufferSize;
    }

}

GranularState Granular::getState() {
    return _state;
}

float Granular::processGrain(int index) {
    
    GranularState * state = &_particles[index];
    
    int prevIdx = floor(state->loopPointer);
    float fracBelow = state->loopPointer - prevIdx;
    float fracAbove = 1.0f-fracBelow;
    

#if INTERPOLATION_METHOD == 'cubic'
    float out = interpolateCubic(
                           _delayBuffer[prevIdx-1],
                           _delayBuffer[prevIdx],
                           _delayBuffer[prevIdx+1],
                           _delayBuffer[prevIdx+2],
                           fracBelow
    );
#else
    float out = _delayBuffer[prevIdx]*fracBelow+_delayBuffer[prevIdx+1]*fracAbove;
#endif
    
//    float out = _delayBuffer[(int) state->loopPointer];
    if (state->loopPointer >= _config.delayBufferSize || state->loopPointer < 0) {
        out = 0;
    }
//    std::cout << "before: " << state->loopPointer << std::endl;
    state->loopPointer = state->loopPointer+this->_config.playbackSpeed;
//    std::cout << "after: " << state->loopPointer << std::endl;
    
    // this is kinda sketchy lmao
    bool grainEnd;
    if (this->_config.playbackSpeed >= 0) {
        grainEnd = state->loopPointer >= state->loopStart+state->loopLength;
//        std::cout << grainEnd << std::endl;
    }
    else {
        grainEnd = state->loopPointer <= state->loopStart-state->loopLength;
    }
    
    
    if (grainEnd) {

        // Wrap Pointer
        if (_config.follow) {
            state->loopStart = state->writePointer-_config.delayTime;
            while (state->loopStart<=0) {
                state->loopStart += _config.delayBufferSize;
            }
            while (state->loopStart>=_config.delayBufferSize) {
                state->loopStart = (int)state->loopStart%_config.delayBufferSize;
            }
        }
        else {
            state->loopStart=_config.loopStart;
        }
        
        // New Randomness Params
        state->loopStart +=(this->_config.startRandomness*ofRandomf()*100000);
        state->loopPointer = state->loopStart;
        
        state->loopLength = _config.loopLength+(this->_config.lengthRandomness*ofRandomf()*10000);
    }
    
    
    // Applying window
    state->windowEnvelope = this->_window.apply(state->loopPointer-state->loopStart,state->loopLength);
//    state.windowEnvelope = 1.0;
    out = out*state->windowEnvelope;
    
    
    return out*.1;
}


float Granular::processLoop() {
    float out = 0;
    for (int i = 0; i < _config.numGrains; i++) {
        out += processGrain(i);
    }
    return out;
}

const float * Granular::getBuffer() {
    return _delayBuffer;
}

const std::vector<GranularState> Granular::getStates() {
    std::vector<GranularState> newVec = this->_particles;
    return newVec;
}

std::string GranularState::toString() {
    std::string out = "writePointer: " + std::to_string(this->writePointer);
    out += "loopPointer: " + std::to_string(this->loopPointer);
    out += "loopStart: " + std::to_string(this->loopPointer);
    return out;
}
