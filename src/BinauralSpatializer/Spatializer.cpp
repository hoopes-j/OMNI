//
//  Spatializer.cpp
//  granular
//
//  Created by Jason Hoopes on 2/28/24.
//

#include "Spatializer.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#define HRTF_DIR "/Users/jhoopes/Desktop/northeastern/sp_2024/capstone/granular_spatialization/hrtfs"

#define AZIMUTH 0
#define ELEVATION 1
#define ITD 2
#define IR_START 3
#define NUM_FREQS 512

#define L 0
#define R 1

#define NUM_CHANNELS 2


// Function to remove carriage returns from a string
void removeCarriageReturns(std::string& str) {
    str.erase(str.length()-2, 2);
}


bool Spatializer::setup() {

    std::string hrtf_csv_path = std::string(HRTF_DIR) + "/omniData.csv";
    
    if (!this->loadSubject(hrtf_csv_path)) {
        std::cerr << "Spatializer Setup: " << "Could not load subject file at " << hrtf_csv_path << std::endl;
        return false;
    }
    
    if (_hrtfMap.size()==0) {
        std::cerr << "Spatializer Setup: " << "Could not load subject file at " << hrtf_csv_path << std::endl;
        return false;
    }
    
    
    HRTF hrtf = this->loadHRTF(180, 75);
    

    return true;
}

bool Spatializer::loadSubject(std::string filePath) {
    
    // Open the CSV file
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return false;
    }


    std::string line;
    // Read each line from the file
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string field;
        std::vector<std::string> fields;
        

        // Split the line into fields using comma as delimiter
        while (std::getline(ss, field, ',')) {
            fields.push_back(field);
        }
        
        int elevation = std::stoi(fields[ELEVATION]);
        int azimuth = std::stoi(fields[AZIMUTH]);

        
        struct HRTF tf;
        std::string itdSymbol = fields[ITD];
//        removeCarriageReturns(itdSymbol);
        tf.itd = std::stof(itdSymbol);
        tf.freqs.resize(NUM_CHANNELS);
        for (int i = 0; i<NUM_FREQS; i++) {
            std::string symL =fields[i+IR_START];
            std::string symR = fields[i+IR_START+NUM_FREQS];
            removeCarriageReturns(symR);
            
            tf.freqs[L].push_back(std::stof(symL));
            tf.freqs[R].push_back(std::stof(symR));
        }
        
        _hrtfMap[{azimuth, elevation}] = tf;
    }

    // Close the file
    file.close();
    
    return true;
}

HRTF Spatializer::loadHRTF(float azimuth, float elevation) {
 
    return _hrtfMap[{azimuth, elevation}];
}

HRTF Spatializer::resample(HRTF x, int len){
 
    int binWidth = x.freqs[L].size() / len;
    int ptr = 0;
    
    HRTF y;
    y.itd = x.itd;
    y.freqs.resize(NUM_CHANNELS);
    
    while (ptr < x.freqs[0].size()) {
        float avgL = 0;
        float avgR = 0;
        for (int i = 0; i < binWidth; i++) {
            avgL = x.freqs[0][ptr+i];
            avgR = x.freqs[1][ptr+i];
        }
        avgL=avgL/(float)binWidth;
        avgR=avgR/(float)binWidth;
        ptr+=binWidth;
        y.freqs[L].push_back(avgL);
        y.freqs[R].push_back(avgR);
    }
    
    return y;
}

