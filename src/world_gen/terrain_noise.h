#pragma once

#include <FastNoise/FastNoise.h>
#include "../physics/physics.h"

using namespace FastNoise;

class Terrain_Noise {
    public:
        float* get_basic_noise(AABB aabb);

    
        
       // Cellular* fast_noise {nullptr};
        
};