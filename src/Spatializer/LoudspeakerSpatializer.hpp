//
//  LoudspeakerSpatializer.hpp
//  granular
//
//  Created by Jason Hoopes on 4/17/24.
//


#pragma once

#include <stdio.h>
#include <math.h>
#include <vector>


class LoudspeakerSpatializer {
public:
    
    /**
        Initialiazes a LoudspeakerSpatializer for a spherical array of n loudspeakers. Assumes that loudspeakers are placed at an equistiant location, from the origin of the spehre, and that all loudpspeakers exist in the same 2D plane.
     */
    bool setup(int numOutput);
    
    /**
        Based on the given angle n the 2d plane, pans a input sample to the internal output buffer for each of the n loudspeakers in this instance. The results of consecutive calls to this function will be summed to the internal output buffer. This allows for realizing several seperate streams of audio, at seperate positions, without having to accumulate the results manually outside of this class.
     
        To retrieve the samples from the output buffer, use "retrieveChannel"
     
        To clear the output buffer,  use "clearOutput()"
     
        To process an individual stream, without storing its output internally, use "process()"
     */
    void processAndStore(float input, float angle);
    void processAndStore(float input);
    
    /**
    Returns the sample held at the given channel
     */
    float getSampleAtChannel(int channel);
    
    /**
     Clears the internal buffer
     */
    void clearOutput();
    
    [[deprecated("Careful When using this overload!! behavior is undefined for parent class")]]
    void updateAngle(float angle);
    void simpleUpdateAngle(float angle);
    
    
private:
    int _numOutputSources;
    int _numInputSources;

    
    float _d; // Angle between each of the loudspeakers
    float _angle; // The current source angle;
    
    /*
     Stores the output samples for a single frame, for each of the number output speakers
     */
    std::vector<float> _output;
    
    std::vector<float> _amplitudes; // save the amplitudes, so we can set the angle and forget, instead of calculating every frame when we don't need to.
    
    
};

