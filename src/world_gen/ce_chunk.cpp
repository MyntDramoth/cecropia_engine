#include "ce_chunk.h"


// CE_Chunk::CE_Chunk(glm::vec3 position) {

//     default_bounds = {position,half_size};
// }

// void CE_Chunk::generate_chunk(glm::vec3 position, SVO *octree) {
//     glm::vec3 size_offset = half_size + position;
//     for(int x = -size_offset.x; x < size_offset.x;x++) {
//         for(int y = -size_offset.y; y < size_offset.y;y++) {
//             for(int z = -size_offset.z; z < size_offset.z;z++) {

//                 //if x,y,z is > 0.5
//                 //add in terrain gen code here


//                  voxel vox = 0;
//                 // return voxel type

               
//                 AABB vox_bounds = {glm::vec3{x,y,z},glm::vec3{0.5f,0.5f,0.5f}};
//                 octree->insert(vox_bounds,vox);


//             }
//         }
//     }
// }