#pragma once



#include "..\types.h"
#include "ce_entity.h"
#include "..\ce_model.h"

//libs
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

//std
#include <memory>
#include <unordered_map>
#include <vector>

struct Transform_2D_Comp {
	glm::vec2 translation{};
	glm::vec2 scale{1.0f,1.0f};
	float rotation;
	glm::mat2 mat2() { 
		const float s = glm::sin(rotation);
		const float c = glm::cos(rotation);

		glm::mat2 rotation_matrix{ {c,s}, {-s,c} };
		glm::mat2 scale_mat{ {scale.x, 0.0f}, {0.0f,scale.y} };
		return rotation_matrix * scale_mat;
	};
};

struct Transform_3D_Comp {
	glm::vec3 translation{};
	glm::vec3 scale{ 1.0f,1.0f,1.0f };
	glm::vec3 rotation;


	//matrix corresponds to translate * Ry * Rx * Rz * scale transform
	//rotation convention uses Tate-Bryan angles with Y(1),X(2),Z(3) axis order
	glm::mat4 mat4();
	glm::mat3 normal_matrix();
};

struct Point_Light_Comp {
	float light_intensity{1.0f};
};

struct Transparency_Comp {
	float alpha{1.0f};
};



enum voxel_interactions {
    IS_FLAMMABLE = 1,
    IS_FILLABLE = 1<<1,
    IS_ROTATABLE = 1<<2,
    IS_AFFECTED_BY_GRAVITY = 1<<3,
};

struct Voxel_Base {
    u16 interaction_mask;
    std::shared_ptr<CE_Model> model;
    Transform_3D_Comp transform_3d{};
};

struct Biome_Base {
    std::vector<voxel> biome_blocks;
    glm::vec2 temperature_range;
    glm::vec2 fluid_level_range;
    glm::vec2 world_depth_range;
};


