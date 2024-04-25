//
//  OmniConfig.h
//  granular
//
//  Created by Jason Hoopes on 4/24/24.
//

#pragma once

#include <string>
#include "yaml_all.hpp"

#include <stdexcept>


#define OMNI_INPUT_CHANNELS 1
#define OMNI_OUTPUT_CHANNELS 2


namespace Omni {

class OfxGranularConfig {
public:
    int numVoices;
};


class OfxSpatialConfig {
public:
    std::string type;
    int numSpatializers;
};


class OfxConfig {
public:
    int inputChannels;
    int outputChannels;
    
    
    
    // Spatializing
    bool useSpatializers;
    OfxSpatialConfig spatial;
    
    
    // ======= Granulation
    OfxGranularConfig granular;

    
    
    
    bool parseYaml(ryml::Tree ryamlTree) {
        
        ryml::ConstNodeRef yamlData = ryamlTree.rootref();  // a const node reference
        
        yamlData["input_channels"] >> inputChannels;
        yamlData["output_channels"] >> outputChannels;
        char spatialKey[] = "spatial";
        
        // ======= Spatial
        yamlData["useSpatial"] >> useSpatializers;
        auto spatialData = yamlData[spatialKey];
        if (useSpatializers) {
            spatialData["type"] >> spatial.type;
            spatialData["num_processors"] >> spatial.numSpatializers;
        } else {
            spatial.type = "none";
        }
        
        // ======= Granular
        char granularKey[] = "granular";
        auto granularData = yamlData[granularKey];
        granularData["num_voices"] >> granular.numVoices;

        
        
        return true;
    }
    
};








};

