//
//  Utils.h
//  granular
//
//  Created by Jason Hoopes on 2/6/24.
//


#ifndef Utils_h
#define Utils_h



inline float interpolateCubic(const float y0, const float y1, const float y2, const float y3, float pct){
    float a0,a1,a2,a3;
    float pct2;

    pct2 = pct*pct;
    a0 = y3 - y2 - y0 + y1;
    a1 = y0 - y1 - a0;
    a2 = y2 - y0;
    a3 = y1;

    return(a0*pct*pct2+a1*pct2+a2*pct+a3);
}

class JWindow {
public:

    JWindow(){
        this->_resolution=512;
        this->_buf.resize(this->_resolution);
        this->_raisedCosine.resize(this->_resolution);
        float size = this->_raisedCosine.size();
        for(int n=0; n<size; n++)
            this->_raisedCosine[n] = -( 1-cosf( 2*M_PI*n/(size-1)) ) / 2.0;
        
        this->_tukey.resize(this->_resolution);
        
        this->name = "raised cosine";
        
        const double alpha = 0.4; // Change this value to adjust the shape of the window
        for (int n = 0; n < size; ++n) {
            
            if (n <= (alpha*_resolution)/2) {
                _tukey[n] = (1-std::cos((M_PI_2*n)/(alpha*_resolution)))/2.0;
            }
            else if (n <= (_resolution)/2) {
                _tukey[n] = 1;
            }
            else {
                _tukey[n] = _tukey[_resolution-n];
            }
        }
        
        if (this->name == "raised cosine") {
            this->_buf = this->_raisedCosine;
        }
    };
    ~JWindow(){};

    inline float apply(float pointer, int grainSize) {

        int windowType = 0;
        if (grainSize < 1) {
            return 0;
        }


//        else { //VIC changed to raised cosine
            //return -( 1-cosf( 2*M_PI*pointer/grainSize) ) / 2.0; //sinf((M_PI*pointer)/grainSize);
            int n = this->_resolution*(abs(pointer)/(float)grainSize);
            return this->_raisedCosine[n];
//        return this->_tukey[n];
//        }

    }
    
    
    float _resolution;
    std::vector<float> _raisedCosine;
    std::vector<float> _tukey;
    std::vector<float> _buf;
    std::string name;
};






#endif /* Utils_h */
