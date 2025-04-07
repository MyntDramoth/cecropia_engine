#pragma once

#include "../types.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

typedef struct hit Hit;
typedef struct body Body;
typedef struct static_body Static_Body;

typedef void (*On_Hit)(Body *self, Body *other, Hit hit);
typedef void (*On_Hit_Static)(Body *self, Static_Body *other, Hit hit);


typedef struct aabb {
	glm::vec3 position;
	glm::vec3 half_size;
} AABB;
/*
struct body {
	AABB aabb;
	glm::vec2 velocity;
	glm::vec2 acceleration;
	On_Hit on_hit;
	On_Hit_Static on_hit_static;
    usize entity_id;
	u8 collision_layer;
	u8 collision_mask;
	bool is_kinematic;
	bool is_active;
};*/

struct static_body {
	AABB aabb;
	u8 collision_layer;
};

struct hit {
	usize other_id;
	f32 time;
	glm::vec2 position;
	glm::vec2 normal;
	bool is_hit;
};

void physics_init(void);
void physics_update(void);
//usize physics_body_create(glm::vec3 position, glm::vec3 size, glm::vec3 velocity, u8 collision_layer, u8 collision_mask, bool is_kinematic, On_Hit on_hit, On_Hit_Static on_hit_static, usize entity_id);
//usize physics_trigger_create(glm::vec3 position, glm::vec3 size, u8 collision_layer, u8 collision_mask, On_Hit on_hit);
Body *physics_body_get(usize index);
Static_Body *physics_static_body_get(usize index);
usize physics_static_body_count();
//usize physics_static_body_create(glm::vec3 position, glm::vec3 size, u8 collision_layer);
bool physics_point_intersect_aabb(glm::vec3 point, AABB aabb);
bool physics_aabb_intersect_aabb(AABB a, AABB b);
AABB aabb_minkowski_difference(AABB a, AABB b);
void aabb_penetration_vector(glm::vec3 r, AABB aabb);
void aabb_min_max(glm::vec3 min, glm::vec3 max, AABB aabb);
Hit ray_intersect_aabb(glm::vec3 position, glm::vec3 magnitude, AABB aabb);
void physics_reset(void);

//void physics_body_destroy(usize body_id);