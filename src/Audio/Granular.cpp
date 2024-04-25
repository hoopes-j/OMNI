//
//  Granular.cpp
//  granular
//
//  Created by Jason Hoopes on 1/17/24.
//

#include "Granular.hpp"



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
    

    if (!_envFollower.setup(15,2)) {
        std::cerr << "Unable to setup Envelope Follower" << std::endl;
        return false;
    }
        
    return true;
}

bool Granular::updateConfig(GranularConfig config) {
    _config = config;
    _state.readPointer = (this->_config.delayBufferSize+(_state.writePointer-_config.delayTime))%this->_config.delayBufferSize;
    return true;
}

void Granular::updatePitch(float spread) {
    
}

bool Granular::distributePitch(std::vector<float> pitches) {
    if (pitches.size() == this->_particles.size()) {
        for (int i = 0; i < this->_particles.size(); i++) {
            this->_particles[i].loopSpeed = pitches[i];
        }
        return true;
    }
    else {
        return false;
    }
}



void Granular::updateBuffer(float value) {
    _delayBuffer[_particles[0].writePointer] = value;
    
    // clear table of existing transients
    for (int i = 0; i < transientPositions.size(); i++) {
        if (_particles[0].writePointer == transientPositions[i]) {
            transientPositions.erase(transientPositions.begin()+i);
        }
    }
    
    // detect transients
    if (_envFollower.detect(value)) {
//        std::cout << "transient detected!! " << std::endl;
        this->addTransient(_particles[0].writePointer-2205); // Subtract 2205 to compensate for the fact that transient will happen at the end of each 5ms block IE, the transient actually started 5 milliseconds ago when it is detected
    }
    

    
    for (int i = 0; i < _config.numGrains; i++) {
        // Should not have seperate write pointer per grain
        _particles[i].writePointer = (_particles[i].writePointer + 1) % _config.delayBufferSize;
    }

}

GranularState Granular::getState() {
    return _state;
}


void Granular::processSetup(float input) {
    _delayBuffer[_particles[0].writePointer] = input;
    
    // clear table of existing transients
    for (int i = 0; i < transientPositions.size(); i++) {
        if (_particles[0].writePointer == transientPositions[i]) {
            transientPositions.erase(transientPositions.begin()+i);
        }
    }
    
    // detect transients
    if (_envFollower.detect(input)) {
//        std::cout << "transient detected!! " << std::endl;
        this->addTransient(_particles[0].writePointer-2205); // Subtract 2205 to compensate for the fact that transient will happen at the end of each 5ms block IE, the transient actually started 5 milliseconds ago when it is detected
    }
}


float Granular::process(float input) {
    
    this->processSetup(input);
    

    float out = this->processGrains();
    
    // ==== Feedback
    if (_config.feedback > 0.f) {
        float safeFeedback;
        float threshold = 0.9;
        if (_config.feedback>threshold) {
            safeFeedback = threshold;
        } else if (_config.feedback<0.f) {
            safeFeedback = 0.f;
        } else {
            safeFeedback = _config.feedback;
        }
        _delayBuffer[_particles[0].writePointer] += out*safeFeedback;
    }
    // ====
            
            
    for (int i = 0; i < _config.numGrains; i++) {
        // TODO: Should not have seperate write pointer per grain
        _particles[i].writePointer = (_particles[i].writePointer + 1) % _config.delayBufferSize;
    }
    
    return out;
}

void Granular::processEach(float input, float * output) {
    
    this->processSetup(input);
    
    float out = this->processGrains(output);
    
    // ==== Feedback
    if (_config.feedback > 0.f) {
        float safeFeedback;
        float threshold = 0.9;
        if (_config.feedback>threshold) {
            safeFeedback = threshold;
        } else if (_config.feedback<0.f) {
            safeFeedback = 0.f;
        } else {
            safeFeedback = _config.feedback;
        }
        _delayBuffer[_particles[0].writePointer] += out*safeFeedback;
    }
    // ====
            
            
    for (int i = 0; i < _config.numGrains; i++) {
        // TODO: Should not have seperate write pointer per grain
        _particles[i].writePointer = (_particles[i].writePointer + 1) % _config.delayBufferSize;
    }
    
}



float Granular::processGrain(int index) {
    

    GranularState * state = &_particles[index];
    
    float out = 0;
    
    if (std::abs(state->loopSpeed-0)>=0.01) {
                    
        int prevIdx = floor(state->loopPointer);
        float fracBelow = state->loopPointer - prevIdx;
        float fracAbove = 1.0f-fracBelow;
        

    #if INTERPOLATION_METHOD == 'cubic'
        out = interpolateCubic(
                               _delayBuffer[prevIdx-1],
                               _delayBuffer[prevIdx],
                               _delayBuffer[prevIdx+1],
                               _delayBuffer[prevIdx+2],
                               fracBelow
        );
    #else
        out = _delayBuffer[prevIdx]*fracBelow+_delayBuffer[prevIdx+1]*fracAbove;
    #endif
        
        if (state->loopPointer >= _config.delayBufferSize || state->loopPointer < 0) {
            out = 0;
        }
        state->loopPointer = state->loopPointer+state->loopSpeed;
        
        // this is kinda sketchy lmao
        bool grainEnd;
        if (state->loopSpeed >= 0) {
            grainEnd = state->loopPointer >= state->loopStart+state->loopLength;
        }
        else {
            grainEnd = state->loopPointer <= state->loopStart-state->loopLength;
        }
        
        if (state->loopPointer-state->loopStart>=state->loopLength/2) {
            state->bkgActive=true;
            state->bgkPointer = state->loopStart;
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
                // Generate Params for Next Grain
                if (_config.warpAmount > 0.5 && transientPositions.size() >= 1) {
                    
                    
                    // Make sure chosen transient is always the most recent one
                    int closest = 1316134912; // Maximum int value
                    int maxIdx = 0;
                    for (int i = 0; i < transientPositions.size(); i++) {
                        float dist = state->writePointer - transientPositions[i];
                        if (dist < closest && dist > 0) {
                            closest = transientPositions[i];
                            maxIdx = i;
                        }
                    }
                    
                    int idx = (std::rand()%transientPositions.size());
                    
                    state->loopStart=transientPositions[idx];
                    
    //                int chance = std::rand() % 2;
    //                if (chance == 0) {
    //                    state->loopStart=transientPositions[idx];
    //                } else {
    //                    state->loopStart=_particles[(index+1)%_particles.size()].loopStart;
    //                }

                } else {
                    state->loopStart=_config.loopStart;
                }
            }
            

            state->loopStart +=(this->_config.startRandomness*ofRandomf()*100000);
            state->loopPointer = state->loopStart;
            
            state->loopLength = _config.loopLength+(this->_config.lengthRandomness*ofRandomf()*10000);
        }
        
        
        // Applying window
        state->windowEnvelope = this->_window.apply(state->loopPointer-state->loopStart,state->loopLength);
    //    state.windowEnvelope = 1.0;
        out = out*state->windowEnvelope;
        
    }
    
    
    return out*.1;
}


float Granular::processGrains() {
    float out = 0;
    for (int i = 0; i < _config.numGrains; i++) {
        if (_particles[i].active) {
            out += processGrain(i);
        }
    }
    return out;
}

float Granular::processGrains(float * output) {
    float out = 0;
    for (int i = 0; i < _config.numGrains; i++) {
        if (_particles[i].active) {
            output[i] = processGrain(i);
            out += output[i];
        }
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
