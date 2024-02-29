//
//  Processor.cpp
//  granular
//
//  Created by Jason Hoopes on 2/28/24.
//

#include "Processor.hpp"
#include "kiss_fft.h"

Processor::Processor(){}
Processor::~Processor() {
    delete[] _fftOut;
    delete[] _ifftOut;
    delete[] _buffer;
}

bool Processor::setup(int bufferSize) {
    
    _bufferSize = bufferSize;
    _fftSize = bufferSize;
    
    int n_fft = bufferSize;
    int is_inverse_fft = 0;
    _kissCfg = kiss_fft_alloc( n_fft ,is_inverse_fft ,0,0 );
    _kissCfgInv = kiss_fft_alloc( n_fft ,1 ,0,0 );
    kiss_fft_cpx * f_in = new kiss_fft_cpx[n_fft];
    kiss_fft_cpx * f_out = new kiss_fft_cpx[n_fft];
    
    for (int i = 0; i < n_fft; i++) {
        f_in[i].r = (rand()%100)/100.0f;
        f_in[i].i = 0.0f;
    }
    
    
    kiss_fft(_kissCfg, f_in, f_out);
    
    
    
    for (int i = 0; i < n_fft; i++) {
        float f = f_out[i].r;
        std::cout << f << std::endl;
    }
    

    _fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_BARTLETT);
    
    _fftOut = new float[_fft->getBinSize()];
    _ifftOut = new float[_fft->getSignalSize()];
    
    
//    if (!_spatializer.setup()) {
//        std::cout << "spatializer was not created" << std::endl;
//        return false;
//    }
    
    
    
    return true;
}



float Processor::process() {
    std::cerr << "Function not Implemented" << std::endl;
    return 0.0f;
}

std::vector<float> Processor::process(float * input) {
    _fft->setSignal(input);
    // Perform FFT
    memcpy(_fftOut, _fft->getAmplitude(), _fft->getBinSize()*sizeof(float));

    if (_fft->getSignalSize()!=_bufferSize) {
        std::cout << "some fucked up shit is goin on" << std::endl;
    }
    
    _fft->setPolar(_fftOut, _fft->getPhase());

    _fft->clampSignal();
        
    memcpy(_ifftOut, _fft->getSignal(), _bufferSize*sizeof(float));
    
    std::vector<float> outVec;
    for (int i =0; i < _fft->getSignalSize(); i++) {
        outVec.push_back(_ifftOut[i]);
    }
    return outVec;
}

std::vector<float> Processor::processKiss(float * input) {
    
    kiss_fft_cpx * f_in = new kiss_fft_cpx[_fftSize];
    kiss_fft_cpx * f_out = new kiss_fft_cpx[_fftSize];
    kiss_fft_cpx * if_out = new kiss_fft_cpx[_fftSize];
    
    for (int i = 0; i < _fftSize; i++) {
        f_in[i].r = input[i];
        f_in[i].i = 0.0f;
    }
    
    
    kiss_fft(_kissCfg, f_in, f_out);
    kiss_fft(_kissCfgInv, f_out, if_out);
    
    
    std::vector<float> outVec;
    for (int i = 0; i < _fftSize; i++) {
        outVec.push_back(if_out[i].r);
    }
    return outVec;
}





//float Processor::update(float sample) {
//    _buffer.pop_back();
////    _buffer.push_
//}
//
//float Processor::update(float * sample) {
//    _buffer.clear();
//}








