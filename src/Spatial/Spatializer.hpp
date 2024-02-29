//
//  Spatializer.hpp
//  granular
//
//  Created by Jason Hoopes on 2/28/24.
//

#ifndef Spatializer_hpp
#define Spatializer_hpp

#include <stdio.h>
#include <vector>
#include <map>




struct HRTF {
    float itd;
    std::vector<std::vector<float>> freqs;
};

typedef std::map<std::pair<int, int>, HRTF> HRTFMap;

class Spatializer {
public:
    Spatializer() {}
    ~Spatializer() {}
    
    bool setup();
    bool loadSubject(std::string filePath);
    
    HRTF loadHRTF(float azimuth, float elevation);
    
    
private:
    HRTFMap _hrtfMap;
};

#endif /* Spatializer_hpp */
