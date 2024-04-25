//
//  SpectralProcessor.hpp
//  fftTest
//
//  Created by Jason Hoopes on 3/25/24.
//

#pragma once

#include <iostream>
#include "fftw3.h"
#include <math.h>
#include "Spatializer.hpp"
#include "ofMain.h"


class SpectralProcessor {
public:
    
    SpectralProcessor();
    ~SpectralProcessor();
    
    bool setup(int nFft, Spatializer * hrtfLoader);
    
    void updatePosition(int azimuth, int elevation);
    void updateHRTF(HRTF hrtf);
    
    void fft();
    void process_spec(); // Perform Fast Convolution
    void ifft();
    
    /**
     Process an input sample, and returns the output sample in 2 channels in the form of an interleaved buffer (of length 2)
     */
    void process(float in, float * outBuffer);
    float process(float in);
    
    
    /**
        Assumes Buffer Size == size of input and outputs
     */

    void cleanup();
    
    inline static float magnitude(float a, float b) {
        return std::sqrtf(std::powf(a,2)+std::powf(b,2));
    }
    
    inline static float phase(float a, float b) {
        return std::atan2f(b,a);
    }
    
    // FFT config
    int _nFft;
    int _hopSize;
    int _overlap;
    int bufferSize;
    
    
    std::vector<float> _windowBuffer;
    
    

    std::vector<float> _fftCollect;
    int _collectPtr;
    int _numCollect;
    
    std::vector<float> _ifftOutput;
    int _ifftOutputSize;
    
    std::vector<float> _fftOutput;
    
    int _fftOutputSize;
    
    std::vector<float> _outBuffer;
    std::vector<float> _outBufferL;
    bool _interleaved;
    int _outBufferLength;
    int _outBufferReadPtr;
    int _outBufferWritePtr;
    
        
    std::vector<float> _real;
    std::vector<float> _imag;
    std::vector<float> _amp;
    std::vector<float> _phase;
    
    
    std::vector<float> fftw_i;
    std::vector<float> fftw_o;
    fftwf_complex * _freq;
    fftwf_plan _forwardP;
    fftwf_plan _inverseP;
    
    std::vector<float> fftInL;
    std::vector<float> fftOutL;
    fftwf_complex * _freqL;
    fftwf_plan _forwardL;
    fftwf_plan _inverseL;
    
    
    // Spatial
    HRTF _hrtf;
    Spatializer * _hrtfLoader;
    
    ofMutex audioMutex;
    
    
};

