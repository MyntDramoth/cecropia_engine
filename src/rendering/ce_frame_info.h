#pragma once

#include "ce_camera.h"
#include "ce_game_object.h"
#include "..\entities\ce_entity.h"

//lib
#include <vulkan/vulkan.h>

#define MAX_LIGHTS 10

struct Point_Lights {
	glm::vec4 position {}; //ignore w
	glm::vec4 color {};
};

struct CE_Frame_Info {
	int frame_index;
	float frame_time;
	VkCommandBuffer command_buffer;
	CE_Camera& camera;
	VkDescriptorSet global_descriptor_set;
	CE_Game_Object::map& game_objects;
	Vox::voxels& used_voxels;
};

struct Global_UBO {
    glm::mat4 projection_matrix{ 1.0f };
    glm::mat4 view_matrix{ 1.0f };
	glm::mat4 inverse_view_matrix{ 1.0f };
    glm::vec4 ambient_light_color{ 1.0f, 1.0f, 1.0f, 0.002f }; // w is light intenstiy
    Point_Lights point_lights[MAX_LIGHTS];
	int num_lights;
};