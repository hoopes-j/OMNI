//
//  SpectralProcessor.cpp
//  fftTest
//
//  Created by Jason Hoopes on 3/25/24.
//

#include "SpectralProcessor.hpp"

SpectralProcessor::SpectralProcessor(){};

// Destructor
SpectralProcessor::~SpectralProcessor()
{
    this->cleanup();
};



bool SpectralProcessor::setup(int nFft, Spatializer * hrtfLoader) {
    
    _nFft = nFft;
    _hopSize = nFft/2;
    _overlap = _nFft-_hopSize;
    
    
    _fftCollect.resize(_nFft*2);
    _ifftOutputSize = _nFft;
    _fftOutputSize = _nFft;
    _fftOutput.resize(_nFft);
    _ifftOutput.resize(_ifftOutputSize);
    
    _outBufferLength=_nFft*4;
    _outBuffer.resize(_outBufferLength); // reasonably large size to avoid buff wrapping overhead
    _outBufferL.resize(_outBufferLength);
    _outBufferWritePtr = _hopSize;
    _outBufferReadPtr = 0;
    
    
    // FFTW
    fftw_i.resize(_nFft);
    fftw_o.resize(_nFft);
    _freq = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * _nFft/2+1);
    _forwardP = fftwf_plan_dft_r2c_1d(_nFft, fftw_i.data(), _freq, FFTW_ESTIMATE);
    _inverseP = fftwf_plan_dft_c2r_1d(_nFft, _freq, fftw_o.data(), FFTW_ESTIMATE);
    
    // R
    fftOutL.resize(_nFft);
    _freqL = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * _nFft/2+1);
    _inverseL = fftwf_plan_dft_c2r_1d(_nFft, _freqL, fftOutL.data(), FFTW_ESTIMATE);
    
    _real.resize(_nFft);
    _imag.resize(_nFft);
    _amp.resize(_nFft);
    _phase.resize(_nFft);

    
    // Window Buffer
    _windowBuffer.resize(_nFft);
    for (int i = 0; i < _nFft; i++){
        _windowBuffer[i] = 0.5*(1-cosf((M_PI*2*i)/_nFft));
    }
    

    // Spatial
    _hrtfLoader = hrtfLoader;
    _hrtf.freqs.resize(2);
    _hrtf.freqs[0].resize(512);
    _hrtf.freqs[1].resize(512);
    _hrtf = _hrtfLoader->loadHRTF(0, 0);
    
    
    
    return true;
}

void SpectralProcessor::updatePosition(int azimuth, int elevation){
   audioMutex.lock();
   _hrtf = _hrtfLoader->loadHRTF(azimuth, elevation);
   audioMutex.unlock();
}


void SpectralProcessor::cleanup()
{
   fftwf_destroy_plan(_forwardP);
   fftwf_destroy_plan(_inverseP);
   fftwf_free(_freq);
}

void SpectralProcessor::fft()
{
    fftwf_execute(_forwardP);
}

void SpectralProcessor::ifft()
{
    fftwf_execute(_inverseP);
    fftwf_execute(_inverseL);
    for (int i = 0; i<_nFft; i++) {
        fftw_o[i] = fftw_o[i]/_nFft*_windowBuffer[i];
        fftOutL[i] = fftOutL[i]/_nFft*_windowBuffer[i];
    }
}

// Manipulate freq data in place
void SpectralProcessor::process_spec()
{
    
    for (int i = 0; i < _nFft/2+1; i++) {
        float a = _freq[i][0];
        float b = _freq[i][1];
        _real[i] = a;
        _imag[i] = b;
        
        _amp[i] = magnitude(a, b);
        _phase[i] = phase(a, b);
        
        
        
        /*
         SPATIAL PROCESSING HERE _ TODO: ABSTRACT
         */


        // compute real value
        float realR, realL;
        realR = realL = cosf(_phase[i])*_amp[i];
        float imag = sinf(_phase[i])*_amp[i];
        
        
        audioMutex.lock();
        float amplitudeR = _amp[i]*_hrtf.freqs[1][i];
        float amplitudeL =  _amp[i]*_hrtf.freqs[0][i];
        audioMutex.unlock();
        float phase = _phase[i];
        
        // Robot
//            r = amplitude;
//            imag = 0;

       
        


        // Whisper
//            float phase = rand()/(float)RAND_MAX * 2.f* M_PI;
        float imagR, imagL;
        realR = cosf(phase) * amplitudeR;
        realL = cosf(phase) * amplitudeL;
        imagR = sinf(phase) * amplitudeR;
        imagL = sinf(phase) * amplitudeL;
        
        
        /*
        PROCESSING
        */
        
        

        _freq[i][0] = realR;
        _freq[i][1] = imagR;
        
        _freqL[i][0] = realL;
        _freqL[i][1] = imagL;
    }
    
}


float SpectralProcessor::process(float in)
{
    _fftCollect[_collectPtr] = in;
    _collectPtr=(_collectPtr+1)%_fftCollect.size();
    _numCollect++;
    
//        // When we have enough samples to perform FFT, do it
    if (_numCollect>=_hopSize) {
        // Copy last n samples to input buffer
        for (int i = 0; i < _nFft; i++) {
            int idx = _collectPtr-_nFft+i;
            if (idx<0) {
                idx+=_fftCollect.size();
            }
            fftw_i[i] = _fftCollect[idx];
        }
        fft();
        process_spec();
        ifft();
        
        for (int i = 0; i<_ifftOutputSize; i++) {
            _outBuffer[_outBufferWritePtr]+=fftw_o[i];
            _outBufferWritePtr=(_outBufferWritePtr+1)%_outBufferLength;
        }
        _outBufferWritePtr=_outBufferWritePtr-_overlap;
        if (_outBufferWritePtr<0)
            _outBufferWritePtr+=_outBuffer.size();
        
        
        _numCollect=0;
    }
    float out = _outBuffer[_outBufferReadPtr];
    float norm = (float)_hopSize/(float)_nFft;
//        out *= norm;
    // Clear sample so it can be written to nextTime;
    _outBuffer[_outBufferReadPtr] = 0;
    _outBufferReadPtr=(_outBufferReadPtr+1)%_outBufferLength;
    
    if (out>=1.0) {
        out=.99;
    }

    return out;
}

void SpectralProcessor::process(float in, float * outBuffer)
{
    
    // Check to make sure the Spatializer is initialized first
    if (_hrtfLoader->numHRTFs()<186) {
        return;
    }
    
    _fftCollect[_collectPtr] = in;
    _collectPtr=(_collectPtr+1)%_fftCollect.size();
    _numCollect++;
    
//        // When we have enough samples to perform FFT, do it
    if (_numCollect>=_hopSize) {
        // Copy last n samples to input buffer
        for (int i = 0; i < _nFft; i++) {
            int idx = _collectPtr-_nFft+i;
            if (idx<0) {
                idx+=_fftCollect.size();
            }
            fftw_i[i] = _fftCollect[idx];
        }
        fft();
        process_spec();
        ifft();
        
        for (int i = 0; i<_ifftOutputSize; i++) {
            _outBuffer[_outBufferWritePtr]+=fftw_o[i];
            _outBufferL[_outBufferWritePtr]+=fftOutL[i];
            _outBufferWritePtr=(_outBufferWritePtr+1)%_outBufferLength;
        }
        _outBufferWritePtr=_outBufferWritePtr-_overlap;
        if (_outBufferWritePtr<0)
            _outBufferWritePtr+=_outBuffer.size();
        
        
        _numCollect=0;
    }
    float outR = _outBuffer[_outBufferReadPtr];
    float outL = _outBufferL[_outBufferReadPtr];
    float norm = (float)_hopSize/(float)_nFft;
//        out *= norm;
    // Clear sample so it can be written to nextTime;
    _outBuffer[_outBufferReadPtr] = 0;
    _outBufferL[_outBufferReadPtr] = 0;
    _outBufferReadPtr=(_outBufferReadPtr+1)%_outBufferLength;
    
    if (outR>=1.0) {
        outR=.99;
    }
    if (outL>=1.0) {
        outL=.99;
    }
    outBuffer[0] = outR;
    outBuffer[1] = outL;
}


