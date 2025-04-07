#pragma once
#include "rendering\my_engine_device.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>

//std
#include <vector>
class CE_Procedural_Model
{
public:

	struct Vertex {
		glm::vec3 position;
		glm::vec3 color;

		static std::vector<VkVertexInputBindingDescription> get_binding_descriptions();
		static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();
	};

	struct Builder {
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
	};

	CE_Procedural_Model(My_Engine_Device& device, const CE_Procedural_Model::Builder& builder);
	~CE_Procedural_Model();

	CE_Procedural_Model(const CE_Procedural_Model&) = delete;
	CE_Procedural_Model& operator=(const CE_Procedural_Model&) = delete;

	void bind(VkCommandBuffer command_buffer);
	void draw(VkCommandBuffer command_buffer);

private:
	void create_vertex_buffers(const std::vector<Vertex>& vertices);
	void create_index_buffers(const std::vector<uint32_t>& indices);

	My_Engine_Device& ce_device;

	VkBuffer vertex_buffer;
	VkDeviceMemory vertex_buffer_memory;
	uint32_t vertex_count;

	bool has_index_buffer{ false };
	VkBuffer index_buffer;
	VkDeviceMemory index_buffer_memory;
	uint32_t index_count;
};

