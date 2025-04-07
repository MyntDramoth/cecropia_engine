#include "ce_model.h"

#include "ce_utils.h"
//libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include<glm/gtx/hash.hpp>

//std
#include <stdexcept>
#include <cassert>
#include <cstring>
#include <unordered_map>

#ifndef ENGINE_DIR
#define ENGINE_DIR "../"
#endif

namespace std {
	template<>
	struct hash<CE_Model::Vertex> {
		size_t operator()(CE_Model::Vertex const& vertex) const {
			size_t seed{ 0 };
			hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

CE_Model::CE_Model(My_Engine_Device& device, const CE_Model::Builder& builder) : ce_device{ device } {
	create_vertex_buffers(builder.vertices);
	create_index_buffers(builder.indices);
}

CE_Model::~CE_Model() {
	
}

void CE_Model::bind(VkCommandBuffer command_buffer) {
	VkBuffer buffers[] = { vertex_buffer->getBuffer()};
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(command_buffer,0,1,buffers,offsets);

	if (has_index_buffer) {
		vkCmdBindIndexBuffer(command_buffer,index_buffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}
}

void CE_Model::draw(VkCommandBuffer command_buffer) {
	if (has_index_buffer) {
		vkCmdDrawIndexed(command_buffer,index_count,1,0,0,0);
	}
	else {
		vkCmdDraw(command_buffer, vertex_count, 1, 0, 0);
	}
}

void CE_Model::create_vertex_buffers(const std::vector<Vertex>& vertices) {
	vertex_count = static_cast<uint32_t>(vertices.size());
	assert(vertex_count >= 3 && "Vertex count must be at least 3");
	VkDeviceSize buffer_size = sizeof(vertices[0]) * vertex_count;
	uint32_t vertex_size = sizeof(vertices[0]);


	CE_Buffer staging_buffer{
		ce_device,
		vertex_size,
		vertex_count,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,

	};

	staging_buffer.map();
	staging_buffer.writeToBuffer((void*)vertices.data());

	vertex_buffer = std::make_unique<CE_Buffer>(
		ce_device,
		vertex_size,
		vertex_count,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


	ce_device.copyBuffer(staging_buffer.getBuffer(), vertex_buffer->getBuffer(), buffer_size);
}

void CE_Model::create_index_buffers(const std::vector<uint32_t>& indices) {
	index_count = static_cast<uint32_t>(indices.size());
	has_index_buffer = index_count > 0;
	
	if (!has_index_buffer) { return; }

	VkDeviceSize buffer_size = sizeof(indices[0]) * index_count;
	uint32_t index_size = sizeof(indices[0]);

	CE_Buffer staging_buffer{
		ce_device,
		index_size,
		index_count,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,

	};

	staging_buffer.map();
	staging_buffer.writeToBuffer((void*)indices.data());

	index_buffer = std::make_unique<CE_Buffer>(
		ce_device,
		index_size,
		index_count,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


	ce_device.copyBuffer(staging_buffer.getBuffer(), index_buffer->getBuffer(), buffer_size);
}



std::unique_ptr<CE_Model> CE_Model::create_model_from_file(My_Engine_Device& device, const std::string& file_path) {
	std::string engine_path = ENGINE_DIR + file_path;
	Builder builder{};
	builder.load_model(engine_path);

	return std::make_unique<CE_Model>(device, builder);
}

void CE_Model::Builder::load_model(const std::string& file_path) {
	
	tinyobj::attrib_t attribute;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, error;
	
	if (!tinyobj::LoadObj(&attribute, &shapes, &materials, &warn, &error, file_path.c_str())) {
		throw std::runtime_error(warn + error);
	}

	vertices.clear();
	indices.clear();

	std::unordered_map<Vertex, uint32_t> unique_vertices{};

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			if (index.vertex_index >= 0) {
				vertex.position = {
					attribute.vertices[3 * index.vertex_index + 0],
					attribute.vertices[3 * index.vertex_index + 1],
					attribute.vertices[3 * index.vertex_index + 2]
				};

				vertex.color = {
						attribute.colors[3 * index.vertex_index + 0],
						attribute.colors[3 * index.vertex_index + 1],
						attribute.colors[3 * index.vertex_index + 2]
				};				
			}

			if (index.normal_index >= 0) {
				vertex.normal = {
					attribute.normals[3 * index.normal_index + 0],
					attribute.normals[3 * index.normal_index + 1],
					attribute.normals[3 * index.normal_index + 2]
				};
			}
			if (index.texcoord_index >= 0) {
				vertex.uv = {
					attribute.texcoords[2 * index.texcoord_index + 0],
					attribute.texcoords[2 * index.texcoord_index + 1]				
				};
			}
			
			if (unique_vertices.count(vertex) == 0) {
				unique_vertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}
			indices.push_back(unique_vertices[vertex]);
		}
	}

}

std::vector<VkVertexInputBindingDescription> CE_Model::Vertex::get_binding_descriptions() {
	std::vector<VkVertexInputBindingDescription> binding_descritions(1);
	binding_descritions[0].binding = 0;
	binding_descritions[0].stride = sizeof(Vertex);
	binding_descritions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return binding_descritions;
}

std::vector<VkVertexInputAttributeDescription> CE_Model::Vertex::get_attribute_descriptions() {
	std::vector<VkVertexInputAttributeDescription> attribute_descritions{};
	attribute_descritions.push_back({ 0,0,VK_FORMAT_R32G32B32_SFLOAT , offsetof(Vertex, position) });
	attribute_descritions.push_back({ 1,0,VK_FORMAT_R32G32B32_SFLOAT , offsetof(Vertex, color) });
	attribute_descritions.push_back({ 2,0,VK_FORMAT_R32G32B32_SFLOAT , offsetof(Vertex, normal) });
	attribute_descritions.push_back({ 3,0,VK_FORMAT_R32G32_SFLOAT , offsetof(Vertex, uv) });

	//above is same as this
	//attribute_descritions[0].binding = 0;
	//attribute_descritions[0].location = 0;
	//attribute_descritions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	//attribute_descritions[0].offset = offsetof(Vertex, position);

	return attribute_descritions;
 }

