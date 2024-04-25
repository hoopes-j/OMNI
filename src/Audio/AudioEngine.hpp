//
//  AudioEngine.hpp
//  granular
//
//  Created by Jason Hoopes on 4/17/24.
//

#pragma once


#define OMNI_L_CHANNEL 0
#define OMNI_R_CHANNEL 1

#define BINAURAL 0
#define LOUDSPEAKER 1

#define BINAURAL_MODE "binaural"
#define LOUDSPEAKER_MODE "loudspeaker"


#include <stdio.h>
#include <vector>
#include "Granular.hpp"
#include "LoudspeakerSpatializer.hpp"
#include "SpatialController.hpp"


class AudioEngine {
public:

    bool setup(
               int sampleRate,
               int numAudioFrames,
               int numInputChannels,
               int numOutputChannels,
               GranularConfig granularConfig,
               const std::string spatializerType,
               int numSpatializers
    );
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
    
    
    // ===== Getters
    /*
     TODO: We should have a better way of doing this - See Below
        - Ideally there would be a config that holds all of this, so we dont need a billion calls from the GUI thread
        - But also shouldn't be passing like a millionthings everytime when we don't need it
        - Same thing goes for the setters too
     */
    
    const float * getBuffer() {return granulator.getBuffer();}
    std::vector<GranularState> getGranularStates() {return granulator.getStates();}
    
    
    // ===== Setters
    
    void setBinauralPosition(int processorNum, int azimuth, int elevation);
    void setLoudspeakerPosition(int processorNum, int position);
    
    
    
protected:

    int _sampleRate;
    int _numAudioFrames;
    int _numOutputChannels;
    int _numInputChannels;
    std::vector<std::vector<float>> _inputBuffer;
    std::vector<std::vector<float>> _outputBuffer;
    
    
    Granular granulator;
    
    
    // ==== SpatialStuff
    std::string _spatializerType;
    int _numSpatializers;

    BinauralController binaural;
    float _binauralOutput[BINAURAL_OUTPUT_CHANNELS];    // Array to accumulate output of spatializers
    float _tmpBinauralOutput[BINAURAL_OUTPUT_CHANNELS]; // tmp array to hold values during accumulation
    LoudspeakerSpatializer loudspeakerSpatializer;
    
    
    std::vector<float> granularOutput;
    

};



