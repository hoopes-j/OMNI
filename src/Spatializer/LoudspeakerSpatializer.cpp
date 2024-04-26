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
    _amplitudes.resize(_numOutputSources);
    
    _d = (M_PI*2)/_numOutputSources;
    _angle = 0;
    this->updateAngle(_angle);
    
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

void LoudspeakerSpatializer::processAndStore(float input)
{
    for (int n = 0; n < _numOutputSources; n++) {
        _output[n] += _amplitudes[n]*input;
    }
}

void LoudspeakerSpatializer::clearOutput()
{
    for (int i = 0; i < _numOutputSources; i++) {
        _output[i] = 0.f;
    }
}

float LoudspeakerSpatializer::getSampleAtChannel(int channel) {
    if (channel>=_numOutputSources) {
        printf("[LoudspeakerSpatializer] Tried to retrieve output from loudspeaker number %d, but this spatializer is only configured for %d loudspeakers", channel, _numOutputSources);
        
    }
    return _output[channel];
}

void LoudspeakerSpatializer::updateAngle(float angle)
{
    // Convert to radians
    angle *=(M_PI/180);
    this->_angle = angle;
    
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
        
        _amplitudes[n] = amplitude;
    }
}

void LoudspeakerSpatializer::simpleUpdateAngle(float angle)
{
    // Reset Amplitudes
    for (int i = 0; i < _numOutputSources; i++) {
        _amplitudes[i]=0;
    }
    float range = 360;
    float delta = range/_numOutputSources;
    for (int i = 0; i < _numOutputSources; i++) {
        float btm = i*delta;
        float top = ((i+1)*delta);
        if (angle < top && angle >= btm) {
            float fracBelow = 1-(angle-btm)/delta;
            float fracAbove = 1-(top-angle)/delta;
            int idxAbove = (i+1)%_numOutputSources;
            _amplitudes[i] = fracBelow;
            _amplitudes[idxAbove] = fracAbove;
        }
    }

}


