//
//  SpatialController.cpp
//  spatializer
//
//  Created by Jason Hoopes on 4/23/24.
//

#include "SpatialController.hpp"

BinauralController::BinauralController(){}
BinauralController::~BinauralController(){this->cleanup();}


bool BinauralController::setup(int numProcessors)
{
    // Load HRTF data
    _hrtfLoader.setup();
    
    _numProcessors = numProcessors;
    for (int i = 0; i < _numProcessors; i++) {
        _binauralProcessors.push_back(new SpectralProcessor());
        _binauralProcessors[i]->setup(BINAURAL_FFT_SIZE, &_hrtfLoader);
        _binauralProcessors[i]->updatePosition(0,0);
    }
    
    _output.resize(BINAURAL_OUTPUT_CHANNELS);
    _tmpOutput.resize(BINAURAL_OUTPUT_CHANNELS);
    

    
    
    return true;
};

void BinauralController::process(int processorNum, float input, float * output)
{
    _binauralProcessors[processorNum]->process(input, output);
}

void BinauralController::processAndStore(int processorNum, float input)
{
    _binauralProcessors[processorNum]->process(input, _tmpOutput.data());
    for (int i = 0; i < BINAURAL_OUTPUT_CHANNELS; i++) {
        _output[i] += _tmpOutput[i];
        _tmpOutput[i] = 0.f; // Reset the tmp sampler holder to prepare for the next call
    }
}


void BinauralController::readOutput(float * output)
{
    for (int i = 0; i < BINAURAL_OUTPUT_CHANNELS; i++) {
        output[i] = _output[i];
    }
}

void BinauralController::clear()
{
    for (int i = 0; i < BINAURAL_OUTPUT_CHANNELS; i++) {
        _output[i]=0.f;
    }
}

void BinauralController::updatePosition(int index, int azimuth, int elevation)
{
    _binauralProcessors[index]->updatePosition(azimuth, elevation);
}

void BinauralController::cleanup()
{
    // Deallocate the memory allocated for SpectralProcessor objects
    for (SpectralProcessor* ptr : _binauralProcessors) {
        delete ptr;
    }
}


