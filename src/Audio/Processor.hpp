//
//  Processor.hpp
//  granular
//
//  Created by Jason Hoopes on 2/28/24.
//

#ifndef Processor_hpp
#define Processor_hpp

#include <stdio.h>
#include <vector>
#include "ofxFft.h"
#include <iostream>
#include "Spatializer.hpp"
#include "kiss_fft.h"

class Processor {
public:
    
    Processor();
    ~Processor();
    
    bool setup(int bufferSize);
    float process();
    std::vector<float> process(float * input);
    std::vector<float> processKiss(float * input);
    float update(float sample);
    float update(float * samples);

private:
    
    Spatializer _spatializer;
    
    ofxFft * _fft;
    kiss_fft_cfg _kissCfg;
    kiss_fft_cfg _kissCfgInv;
    float * _buffer;
    float * _fftOut;
    float * _ifftOut;
    int _bufferSize;
    int _sampleRate;
    int _fftSize;
    
};

#endif /* Processor_hpp */
