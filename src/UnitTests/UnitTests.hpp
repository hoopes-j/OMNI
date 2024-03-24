//
//  UnitTests.hpp
//  granular
//
//  Created by Jason Hoopes on 3/23/24.
//

#pragma once

#include <stdio.h>
#include "EnvelopeFollowerTests.hpp"

class UnitTests {
public:
    UnitTests(){};
    ~UnitTests(){};
    
    void runAll() {
        
        
        EnvelopeFollowerTests tests;
        tests.run();
    }
    
    
    
    
};




