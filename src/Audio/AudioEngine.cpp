//
//  AudioEngine.cpp
//  granular
//
//  Created by Jason Hoopes on 4/17/24.
//

#include "AudioEngine.hpp"


bool AudioEngine::setup(
                        int sampleRate,
                        int numAudioFrames,
                        int numInputChannels,
                        int numOutputChannels,
                        GranularConfig granularConfig,
                        const std::string spatializerType,
                        int numSpatializers
){
    
    this->_sampleRate = sampleRate;
    this->_numAudioFrames = numAudioFrames;
    this->_numInputChannels = numInputChannels;
    this->_numOutputChannels = numOutputChannels;
    
    // === Spatial
    this->_spatializerType = spatializerType;
    this->_numSpatializers = numSpatializers;
    
    
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
        std::cerr << "[AudioEngine::setup] Unable to setup granular instance" << std::endl;
        return false;
    }
    granularOutput.resize(granularConfig.numGrains);
    
    
    // LOUDSPEAKER specific setup
    if (_spatializerType == LOUDSPEAKER_MODE) {
        if (!loudspeakerSpatializer.setup(2)) {
            std::cerr << "[AudioEngine::setup] Unable to setup Loudspeaker spatializer" << std::endl;
            return false;
        }
        loudspeakerSpatializer.processAndStore(1.0f, M_PI/2);
        std::cout << "[AudioEngine::setup] " << "using loudspeaker spatializer" << std::endl;
    }
    // BINAURAL specific setup
    else if (_spatializerType == BINAURAL_MODE) {
        if (!binaural.setup(_numSpatializers)) {
            std::cerr << "[AudioEngine::setup] Unable to setup Loudspeaker spatializer" << std::endl;
            return false;
        }
        if (_numOutputChannels != BINAURAL_OUTPUT_CHANNELS) {
            std::cerr << "[AudioEngine::setup] Using Binaural Spatialization, but number of output channels is not equal to 2";
            return false;
        }
        std::cout << "[AudioEngine::setup] " << "Using Binaural Spatializer" << std::endl;
    }
    // No Spatialization
    else {
        std::cout << "[AudioEngine::setup] " << "Spatializers are disabled" << std::endl;
    }


    return true;
}


void AudioEngine::process()
{
    for (int frame = 0; frame < _numAudioFrames; frame++) {
        // Assume mono input for now
        float input = this->getInput(frame, 0);
        
        float out = 0;
        granulator.processEach(input, granularOutput.data());
        
        if (_spatializerType == LOUDSPEAKER_MODE) {
            loudspeakerSpatializer.clearOutput();
            loudspeakerSpatializer.processAndStore(out, 0);
            for (int channel = 0; channel < _numOutputChannels; channel++) {
                float spatialized = loudspeakerSpatializer.getSampleAtChannel(channel);
                this->setOutput(spatialized, frame, channel);
            }
        } else if (_spatializerType == BINAURAL_MODE) {
            for (int s = 0; s < binaural.numProcessors(); s++) {
                binaural.process(s, granularOutput[s], &_tmpBinauralOutput[0]);
                _binauralOutput[OMNI_L_CHANNEL]+=_tmpBinauralOutput[OMNI_L_CHANNEL];
                _binauralOutput[OMNI_R_CHANNEL]+=_tmpBinauralOutput[OMNI_R_CHANNEL];
                _tmpBinauralOutput[OMNI_L_CHANNEL] = 0;
                _tmpBinauralOutput[OMNI_R_CHANNEL] = 0;
            }
            
            this->setOutput(_binauralOutput[OMNI_L_CHANNEL], frame, OMNI_L_CHANNEL);
            this->setOutput(_binauralOutput[OMNI_R_CHANNEL], frame, OMNI_R_CHANNEL);
            // Reset the values to prepare for accumulation in next call
            _binauralOutput[OMNI_L_CHANNEL] = 0;
            _binauralOutput[OMNI_R_CHANNEL] = 0;
        } else {
            for (int channel = 0; channel < _numOutputChannels; channel++) {
                this->setOutput(granularOutput[channel], frame, channel);
            }
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

// ===== Setters for GUI (see notes in header function

void AudioEngine::setBinauralPosition(int processorNum, int azimuth, int elevation) {
    binaural.updatePosition(processorNum, azimuth, elevation);
}

void AudioEngine::setLoudspeakerPosition(int processorNum, int position) {
    loudspeakerSpatializer.updateAngle(position);
}





