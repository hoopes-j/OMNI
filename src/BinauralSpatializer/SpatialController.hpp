//
//  SpatialController.hpp
//  spatializer
//
//  Created by Jason Hoopes on 4/23/24.
//
#pragma once

#include <stdio.h>
#include "SpectralProcessor.hpp"
#include "Spatializer.hpp"


#define BINAURAL_FFT_SIZE 512
#define BINAURAL_OUTPUT_CHANNELS 2

class BinauralController {
    
public:
    
    BinauralController();
    ~BinauralController();

    bool setup(int numProcessors);
    
    /**
        Spatializes an input signal with the spatializer at the given index
     */
    void process(int processorNum, float input, float * output);
    /**
        Spatializes an input signal with the spatializer at the given index, and accumulates the output in this instance's output buffer
     */
    void processAndStore(int processorNum, float input);
    /**
        Fills the given pointer with the samples currently stored in this instance's output.
     */
    void readOutput(float * output);
    /**
        Resets all the stored outputs back to 0.
     */
    void clear();
    
    /**
     Update the position of a single processor
     */
    void updatePosition(int index, int azimuth, int elevation);
    
    /**
    Cleans up any members of this instance
     */
    void cleanup();
    
    /**
    The number of active processors in this instance
     */
    int numProcessors() {return _numProcessors;};
    
    
    
private:
    std::vector<SpectralProcessor *> _binauralProcessors;
    int _numProcessors;
    
    std::vector<float> _output;
    std::vector<float> _tmpOutput;
    
    Spatializer _hrtfLoader;
};
