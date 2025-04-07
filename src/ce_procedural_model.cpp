#include "ce_procedural_model.h"

#include <cassert>
#include <cstring>

CE_Procedural_Model::CE_Procedural_Model(My_Engine_Device& device, const CE_Procedural_Model::Builder& builder) : ce_device{ device } {
	create_vertex_buffers(builder.vertices);
	create_index_buffers(builder.indices);
}

CE_Procedural_Model::~CE_Procedural_Model() {
	vkDestroyBuffer(ce_device.device(), vertex_buffer, nullptr);
	vkFreeMemory(ce_device.device(), vertex_buffer_memory, nullptr);
	if (has_index_buffer) {
		vkDestroyBuffer(ce_device.device(), index_buffer, nullptr);
		vkFreeMemory(ce_device.device(), index_buffer_memory, nullptr);
	}
}

void CE_Procedural_Model::bind(VkCommandBuffer command_buffer) {
	VkBuffer buffers[] = { vertex_buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);

	if (has_index_buffer) {
		vkCmdBindIndexBuffer(command_buffer, index_buffer, 0, VK_INDEX_TYPE_UINT32);
	}
}

void CE_Procedural_Model::draw(VkCommandBuffer command_buffer) {
	if (has_index_buffer) {
		vkCmdDrawIndexed(command_buffer, index_count, 1, 0, 0, 0);
	}
	else {
		vkCmdDraw(command_buffer, vertex_count, 1, 0, 0);
	}
}

void CE_Procedural_Model::create_vertex_buffers(const std::vector<Vertex>& vertices) {
	vertex_count = static_cast<uint32_t>(vertices.size());
	assert(vertex_count >= 3 && "Vertyex count must be at least 3");
	VkDeviceSize buffer_size = sizeof(vertices[0]) * vertex_count;

	ce_device.createBuffer(
		buffer_size,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		vertex_buffer, vertex_buffer_memory
	);

	void* data;
	vkMapMemory(ce_device.device(), vertex_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, vertices.data(), static_cast<size_t>(buffer_size));
	vkUnmapMemory(ce_device.device(), vertex_buffer_memory);
	
}

void CE_Procedural_Model::create_index_buffers(const std::vector<uint32_t>& indices) {
	index_count = static_cast<uint32_t>(indices.size());
	has_index_buffer = index_count > 0;

	if (!has_index_buffer) { return; }

	VkDeviceSize buffer_size = sizeof(indices[0]) * index_count;
	
	ce_device.createBuffer(
		buffer_size,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		index_buffer, index_buffer_memory
	);

	void* data;
	vkMapMemory(ce_device.device(), index_buffer_memory, 0, buffer_size, 0, &data);
	memcpy(data, indices.data(), static_cast<size_t>(buffer_size));
	vkUnmapMemory(ce_device.device(), index_buffer_memory);

}

std::vector<VkVertexInputBindingDescription> CE_Procedural_Model::Vertex::get_binding_descriptions() {
	std::vector<VkVertexInputBindingDescription> binding_descritions(1);
	binding_descritions[0].binding = 0;
	binding_descritions[0].stride = sizeof(Vertex);
	binding_descritions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return binding_descritions;
}

std::vector<VkVertexInputAttributeDescription> CE_Procedural_Model::Vertex::get_attribute_descriptions() {
	std::vector<VkVertexInputAttributeDescription> attribute_descritions(2);
	attribute_descritions[0].binding = 0;
	attribute_descritions[0].location = 0;
	attribute_descritions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attribute_descritions[0].offset = offsetof(Vertex, position);

	attribute_descritions[1].binding = 0;
	attribute_descritions[1].location = 1; //location used in vertex shader
	attribute_descritions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attribute_descritions[1].offset = offsetof(Vertex, color);
	return attribute_descritions;
}