//
//  AudioEngine.cpp
//  granular
//
//  Created by Jason Hoopes on 4/17/24.
//

#include "AudioEngine.hpp"


bool AudioEngine::setup(int sampleRate, int numAudioFrames, int numInputChannels, int numOutputChannels, GranularConfig granularConfig)
{
    
    this->_sampleRate = sampleRate;
    this->_numAudioFrames = numAudioFrames;
    this->_numInputChannels = numInputChannels;
    this->_numOutputChannels = numOutputChannels;
    
    
    // === Allocate Audio I/O buffers
    this->_inputBuffer.resize(_numInputChannels);
    for (int inChannel = 0; inChannel < numInputChannels; inChannel++) {
        _inputBuffer[inChannel].resize(_numAudioFrames);
    }
    this->_outputBuffer.resize(_numOutputChannels);
    for (int outChannel = 0; outChannel < numOutputChannels; outChannel++) {
        _outputBuffer[outChannel].resize(_numAudioFrames);
    }
    
    
    // ====== Granulator
    if (!granulator.setup(granularConfig)) {
        std::cerr << "[AudioEngine] Unable to setup granular instance" << std::endl;
        return false;
    }
    
    // ====== Spatializer
#if SPATIALIZER == LOUDSPEAKER
    if (!loudspeakerSpatializer.setup(2)) {
        std::cerr << "[AudioEngine] Unable to setup Loudspeaker spatializer" << std::endl;
        return false;
    }
    
    loudspeakerSpatializer.processAndStore(1.0f, M_PI/2);
#endif
    
    
    

    return true;
}


void AudioEngine::process()
{
    for (int frame = 0; frame < _numAudioFrames; frame++) {
        // Assume mono input for now
        float in = this->getInput(frame, 0);
        
        float out = 0;
        out = granulator.process(in);
        
        loudspeakerSpatializer.clearOutput();
        loudspeakerSpatializer.processAndStore(out, 0);
        
        for (int channel = 0; channel < _numOutputChannels; channel++) {
            float spatialized = loudspeakerSpatializer.getSampleAtChannel(channel);
            this->setOutput(out, frame, channel);
        }
    }
}

void AudioEngine::updateConfig(GranularConfig newGranularConfig)
{
    granulator.updateConfig(newGranularConfig);
}

void AudioEngine::setInput(float sample, int frame, int channel)
{
    _inputBuffer[channel][frame] = sample;
}

void AudioEngine::setOutput(float sample, int frame, int channel)
{
    _outputBuffer[channel][frame] = sample;
}

float AudioEngine::getOutput(int frame, int channel)
{
    return _outputBuffer[channel][frame];
}

float AudioEngine::getInput(int frame, int channel)
{
    return _inputBuffer[channel][frame];
}







