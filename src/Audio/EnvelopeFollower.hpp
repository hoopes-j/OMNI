//
//  EnvelopeFollower.hpp
//  granular
//
//  Created by Jason Hoopes on 3/23/24.
//

#pragma once

#include <stdio.h>
#include <vector>
#include <queue>

#define ANALYSIS_BLOCK_MS 5
#define SAMPLE_RATE 44100
#define ANALYSIS_BLOCK_SMP 2205
#define NUM_LOOKBACK_BLOCKS 3

class EnvFollower {
public:
    EnvFollower() {}
    ~EnvFollower() {}
    
    bool setup(float analysisSizeMS = ANALYSIS_BLOCK_MS, float threshold = 1.5) {
        _threshold = threshold;
        _n = (analysisSizeMS/1000)*SAMPLE_RATE;
        t_2 = 0;
        t_1 = 0;
        _writePtr = 0;
        return true;
    }
    
    bool detectBlock() {
        
        // Calculate RMS
        float rms = 0;
        for (int i = 0; i < _n; i++) {
            rms+=powf(_analysisBlock[i],2);
        }
        rms=sqrtf(rms/_n);
        
        bool isTransient = rms>t_1 && rms>t_2*_threshold;
        
        // shift historical levels
        t_2 = t_1;
        t_1 = rms;
        
        return isTransient;
    }
    
    bool detect(float value) {
        _analysisBlock[_writePtr] = value;
        _writePtr++;
        if (_writePtr>=_n) {
            _writePtr = 0;
            return detectBlock();
        }
        
        return false;
    }
    
    void setThreshold(float threshold) {
        _threshold = threshold;
    }
    
    void cleanup();
    
    int _n;
    int _writePtr;
    float _analysisBlock[ANALYSIS_BLOCK_SMP];
    float _levelHistory[NUM_LOOKBACK_BLOCKS];
    // 1 time step in the past
    float t_1;
    // 2 time steps in the past
    float t_2;
    // Threshold for second
    float _threshold;
    std::vector<int> _transientIndices;
    
    
    
    
};

