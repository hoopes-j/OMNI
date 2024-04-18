//
//  LoudspeakerSpatializer.cpp
//  granular
//
//  Created by Jason Hoopes on 4/17/24.
//

#include "LoudspeakerSpatializer.hpp"


bool LoudspeakerSpatializer::setup(int numSpeakers)
{
    _numOutputSources = numSpeakers;
    _output.resize(_numOutputSources);
    
    _d = (M_PI*2)/_numOutputSources;
    
    return true;
}

void LoudspeakerSpatializer::processAndStore(float input, float angle)
{
    float twoPi = M_PI*2;
    float theta = angle;
    while (theta<0) {
        theta+=twoPi;
    }
    while (theta>twoPi) {
        theta-=twoPi;
    }
    for (int n = 0; n < _numOutputSources; n++) {
        float sourceAngle = _d*(float)n;
        
        float thetaDiff = 0;
        if (sourceAngle!=0.f) {
            thetaDiff=abs(theta-sourceAngle);
        } else {
            thetaDiff=std::min(theta,abs(twoPi-theta));
        }

        
        float amplitude = 0.f;
        if (thetaDiff==0) {
            amplitude=1.f;
        } else if (thetaDiff<_d) {
            amplitude=abs(1.f/(_d/thetaDiff));
        }
        
        _output[n] += amplitude*input;
    }
}

void LoudspeakerSpatializer::clearOutput()
{
    _output.clear();
}

float LoudspeakerSpatializer::getSampleAtChannel(int channel) {
    if (channel>=_numOutputSources) {
        printf("[LoudspeakerSpatializer] Tried to retrieve output from loudspeaker number %d, but this spatializer is only configured for %d loudspeakers", channel, _numOutputSources);
        
    }
    return _output[channel];
}


