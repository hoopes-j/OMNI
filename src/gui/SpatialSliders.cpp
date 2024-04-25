//
//  SpatialSliders.cpp
//  granular
//
//  Created by Jason Hoopes on 4/24/24.
//

#include "SpatialSliders.hpp"


void SpatialSliders::setup(int num, std::string type) {
    this->numSpatializers = num;
    this->spatializerType = type;
    
    params.setName(spatializerType);
    
    spatializerGroups.resize(numSpatializers);
    if (spatializerType=="binaural") {
        azimuths.resize(numSpatializers);
        elevations.resize(numSpatializers);
        for (int i = 0; i < numSpatializers; i++) {
            std::string groupName = "spatializer"+std::to_string(i);
            spatializerGroups[i].setName(groupName);
            spatializerGroups[i].add(azimuths[i].set("azimuth", 0, -180, 165));
            spatializerGroups[i].add(elevations[i].set("elevation", 0, -45, 60));
            params.add(spatializerGroups[i]);
        }
    }
    if (spatializerType=="loudspeaker") {
        positions.resize(numSpatializers);
        for (int i = 0; i < numSpatializers; i++) {
            std::string groupName = "spatializer"+std::to_string(i);
            spatializerGroups[i].setName(groupName);
            spatializerGroups[i].add(positions[i].set("position", 0, 0, 360));
            params.add(spatializerGroups[i]);
        }
    }
    
}
    
