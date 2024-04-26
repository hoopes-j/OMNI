//
//  LoudspeakerController.hpp
//  granular
//
//  Created by Jason Hoopes on 4/25/24.
//

#pragma once

#include <stdio.h>
#include "LoudspeakerSpatializer.hpp"


class LoudspeakerController {
    
public:
    
    LoudspeakerController();
    ~LoudspeakerController();

    bool setup(int numProcessors, int numOutputs);
    
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
        Retrieves the sample at a channel, summed over all the current processors
     */
    float getSampleAtChannel(int channel);
        
    /**
        Resets all the stored outputs back to 0.
     */
    void clear(int processorNum);
    void clearAll();
    
    /**
     Update the position of a single processor
     */
    void updatePosition(int index, int angle);
    
    /**
    Cleans up any members of this instance
     */
    void cleanup();
    
    /**
    The number of active processors in this instance
     */
    int numProcessors() {return _numProcessors;};
    
    
    
private:
    std::vector<LoudspeakerSpatializer *> _loudspeakerProcessors;
    int _numProcessors;
    int _numOutputs;
    
    std::vector<float> _output;
    std::vector<float> _tmpOutput;
    
};
