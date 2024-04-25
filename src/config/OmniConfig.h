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
    std::vector<std::vector<int>> mappings;
};

class OfxGraphicsConfig {
public:
    bool renderSpatializer;
    std::string bufferRenderType;
    
    int bgColor[4];
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
    
    OfxGraphicsConfig graphics;

    
    
    
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
            spatial.mappings.resize(spatial.numSpatializers);
            spatial.mappings[0].resize(4);
            spatial.mappings[1].resize(4);
            for (int j=0;j<4;j++) {
                int idx, idx2;
                spatialData["proc1_mapping"][j] >> idx;
                spatialData["proc2_mapping"][j] >> idx2;
                spatial.mappings[0][j] = idx;
                spatial.mappings[1][j] = idx2;
            }
//            for (int i=0;i<spatial.numSpatializers;i++) {
//                std:string name = std::string("proc") + std::to_string(i+1) + std::string("_mapping");
//                for (int j=0;j<4;j++) {
//                    int idx;
//                    spatialData[name.c_str()][i] >> idx;
//                    spatial.mappings[i][j] = idx;
//                }
//            }


            
        } else {
            spatial.type = "none";
        }
        
        // ======= Granular
        char granularKey[] = "granular";
        auto granularData = yamlData[granularKey];
        granularData["num_voices"] >> granular.numVoices;
        
        // ======= Graphics
        char graphicsKey[] = "graphics";
        auto graphicsData = yamlData[graphicsKey];
        graphicsData["renderSpatializer"] >> graphics.renderSpatializer;
        graphicsData["bufferRenderType"] >> graphics.bufferRenderType;
        for (int i=0;i<4;i++) {
            graphicsData["background_color"][i] >> graphics.bgColor[i];
        }


        
        
        return true;
    }
    
};








};

