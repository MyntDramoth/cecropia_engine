#pragma once

#include "rendering\my_engine_device.h"
#include "rendering\ce_buffer.h"
#include "rendering\ce_texture.h"
//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>

//std
#include <memory>
#include <vector>

class CE_Model
{
public:

	struct Vertex{
		glm::vec3 position{};
		glm::vec3 color{};
		glm::vec3 normal{};
		glm::vec2 uv{};

		static std::vector<VkVertexInputBindingDescription> get_binding_descriptions();
		static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();

		bool operator == (const Vertex& other) const {
			return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
		};
	};

	struct Builder {
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};

		void load_model(const std::string& file_path);
	};

	CE_Model(My_Engine_Device& device, const CE_Model::Builder &builder);
	~CE_Model();

	CE_Model(const CE_Model&) = delete;
	CE_Model &operator=(const CE_Model&) = delete;

	static std::unique_ptr<CE_Model> create_model_from_file(My_Engine_Device& device, const std::string& file_path);


	void bind(VkCommandBuffer command_buffer);
	void draw(VkCommandBuffer command_buffer);

private:
	void create_vertex_buffers(const std::vector<Vertex> &vertices);
	void create_index_buffers(const std::vector<uint32_t>& indices);

	My_Engine_Device& ce_device;

	std::unique_ptr<CE_Buffer> vertex_buffer;
	uint32_t vertex_count;

	bool has_index_buffer{false};
	std::unique_ptr<CE_Buffer> index_buffer;
	uint32_t index_count;

};

