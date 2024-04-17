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
#include "PitchManager.hpp"

#define INTERPOLATION_METHOD 'cubic'

class GranularConfig {
public:
    
    int   numGrains;
    int   sampleRate;
    int   delayTime;
    float feedback;
    int   delayBufferSize;
    
    bool  loopOn;
    float loopLength;
    bool  follow;
    float loopStart;
    float playbackSpeed;
    
    // Warping
    float warpAmount;
    int numWarpPoints;
    
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
    float bgkPointer;
    bool bkgActive;
    float loopStart;
    float loopLength;
    float loopSpeed;
    bool active;
    
    float windowEnvelope;
    
    GranularState() {
        this->active = true;
        this->loopSpeed = 1.0;
        this->bkgActive = false;
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
    std::set<int> transients;
    

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
    
    


    
    /**
        Processes the next sample, and returns the output of each grain in each respective index in output.
        grainOutputs should be a pointer with length = numGrains.
        input is a single floating point value as input.
     */
    void processEach(float input, float * grainOutputs);
    /**
        Writes the input sample to the circular buffer, processes each grain, and returns the sum of their outputs.
     */
    float process(float input);
    
    float processGrain(int index);
    
    float processLoop();
    void processGrains(float * output);
    
    const float * getBuffer();
    
    GranularState getState();
    const std::vector<GranularState> getStates();
    
    // Pitch
    bool distributePitch(std::vector<float> pitches);
    void updatePitch(float spread);
    
    
    void addTransient(int location) {
        int transientLimit = _config.numWarpPoints == 0 ? 1 : _config.numWarpPoints;
        if (transientPositions.size()>=transientLimit) {
            transientPositions.erase(transientPositions.begin());
            int i = 0;
        }
        transientPositions.push_back(location);
    };
    
    
    
    std::vector<int> getTransients() {return transientPositions;}
    JWindow getWindow() {return _window;}
    
    
private:
    void processSetup(float input);
    

};


#endif /* Granular_hpp */
