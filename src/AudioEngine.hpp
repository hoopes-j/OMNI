//
//  AudioEngine.hpp
//  granular
//
//  Created by Jason Hoopes on 4/17/24.
//

#pragma once


#define BINAURAL 0
#define LOUDSPEAKER 1

#define SPATIALIZER 1

#include <stdio.h>
#include <vector>
#include "Granular.hpp"
#include "LoudspeakerSpatializer.hpp"


class AudioEngine {
public:

    bool setup(int sampleRate, int numAudioFrames, int numInputChannels, int numOutputChannels, GranularConfig granularConfig);
    float process(const float input);
    void process(const float * inputBuffer, float * outputBuffer);
    void process();
    
    void setInput(float sample, int frame, int channel);
    void setOutput(float sample, int frame, int channel);
    float getInput(int frame, int channel);
    float getOutput(int frame, int channel);
    
    int framesPerBlock() {return _numAudioFrames;};
    int numOutputChannels() {return _numOutputChannels;};
    int numInputChannels() {return _numInputChannels;};
    
    // =======
    /**
            Updates the internal state and configuration of any children of this audio engine.
     */
    void updateConfig(GranularConfig newGranularConfig);
    
protected:

    int _sampleRate;
    int _numAudioFrames;
    int _numOutputChannels;
    int _numInputChannels;
    std::vector<std::vector<float>> _inputBuffer;
    std::vector<std::vector<float>> _outputBuffer;
    
    
    Granular granulator;
    LoudspeakerSpatializer loudspeakerSpatializer;
    
    std::vector<float> granularOutput;
    

};



