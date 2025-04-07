#pragma once

#include "physics.h"

struct Ray {
    AABB ray_bounds;
    glm::vec3 start_pos, end_pos;
};