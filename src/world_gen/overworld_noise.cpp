#include "terrain_noise.h"
/*
float* Terrain_Noise::get_basic_noise(AABB aabb) {
    float* result;
    int size = (aabb.half_size.x * 2);
    float frequency = 0.25f;
    int seed = 37774;
    
    fast_noise->GenUniformGrid3D(result,
    (aabb.position.x - aabb.half_size.x),
    (aabb.position.y - aabb.half_size.y),
    (aabb.position.z - aabb.half_size.z),
    size,size,size,frequency,seed);


    return result;
}*/