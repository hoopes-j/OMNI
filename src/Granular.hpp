//
//  Granular.hpp
//  granular
//
//  Created by Jason Hoopes on 1/17/24.
//

#ifndef Granular_hpp
#define Granular_hpp

#include <stdio.h>
#include "ofMain.h"
#include <vector>
#include <stdlib.h>
#include <iostream>
#include "Utils.h"
#include "EnvelopeFollower.hpp"

#define INTERPOLATION_METHOD 'cubic'

class GranularConfig {
public:
    int numGrains;
    int sampleRate;
    int delayTime;
    float feedback;
    int delayBufferSize;
    
    bool loopOn;
    float loopLength;
    bool follow;
    float loopStart;
    float playbackSpeed;
    float warpAmount;
    
    float startRandomness;
    float lengthRandomness;
    
    GranularConfig() {
        this->delayTime = NULL;
        this->sampleRate = NULL;
        this->feedback = NULL;
        this->follow = false;
    }
};

class GranularState {
public:
    int writePointer;
    int readPointer;
    int delayTimeSamps;
    
    float loopPointer;
    float loopStart;
    float loopLength;
    float loopSpeed;
    bool active;
    
    float windowEnvelope;
    
    GranularState() {
        this->active = true;
        this->loopSpeed = 1.0;
    }
    
    std::string toString();
    
};

class Granular {
    GranularConfig _config;
    GranularState _state;
    JWindow _window;
    std::vector<GranularState> _particles;
    float * _delayBuffer;
    int _delayBufferSize;
    
    EnvFollower _envFollower;
    std::vector<int> transientPositions;
    
    
public:
    Granular(){};
    ~Granular(){
        delete[] _delayBuffer;
    };
//    bool setup(int maxDelayLength, int delayTime, int sampleRate);
    bool setup(GranularConfig granConfig);
    bool updateConfig(GranularConfig granConfig); 
    float processDelay(float input);
    
    void updateBuffer(float value);
    float processLoop();
    
    float processGrain(int index);
    
    const float * getBuffer();
    
    GranularState getState();
    const std::vector<GranularState> getStates();
    
    bool distributePitch(std::vector<float> pitches);
    
    void addTransient(int location) {
        transientPositions.push_back(location);
    };
    
    std::vector<int> getTransients() {
        return transientPositions;
    }
    
    

};


#endif /* Granular_hpp */
