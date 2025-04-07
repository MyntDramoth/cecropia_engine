#pragma once
#include "..\types.h"
#include <bitset>
#include <limits>
#include <vector>
#include <cstdint>

using entity = u32;
const entity MAX_ENTITIES = 5000;

using voxel = u32;
const voxel MAX_VOXELS = 5000/*std::numeric_limits<u32>::max()*/; //don't go too large it will crash everything

//defines the components attached to an entity and the max allowed
using component_type = u8;
const component_type MAX_COMPONENTS = 32;

using vox_component_type = u8;
const vox_component_type MAX_VOXEL_COMPONENTS = 32;

using signature = std::bitset<MAX_COMPONENTS>;
using vox_signature = std::bitset<MAX_VOXEL_COMPONENTS>;

struct Vox {
    using voxels = std::vector<voxel>;
};