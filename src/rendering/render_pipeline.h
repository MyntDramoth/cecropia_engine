#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include "my_engine_device.h"

struct pipeline_config_info {	
	
	pipeline_config_info(const pipeline_config_info&) = delete;
	pipeline_config_info& operator=(const pipeline_config_info&) = delete;

	pipeline_config_info() = default;

	std::vector<VkVertexInputBindingDescription> binding_descriptions;
	std::vector<VkVertexInputAttributeDescription> attribute_descriptions;
	VkPipelineViewportStateCreateInfo viewport_info;
	VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
	VkPipelineRasterizationStateCreateInfo rasterization_info;
	VkPipelineMultisampleStateCreateInfo multisample_info;
	VkPipelineColorBlendStateCreateInfo color_blend_info;
	VkPipelineColorBlendAttachmentState color_blend_attachment;
	VkPipelineDepthStencilStateCreateInfo depth_stencil_info;
	std::vector<VkDynamicState> dynamic_state_enables;
	VkPipelineDynamicStateCreateInfo dynamic_state_info;

	VkPipelineLayout pipeline_layout = nullptr;
	VkRenderPass render_pass = nullptr;
	uint32_t subpass = 0;
};

class Render_Pipeline {

public:
	

	Render_Pipeline(My_Engine_Device &device,const std::string &vert_path, const std::string &frag_path, const pipeline_config_info& config);
	~Render_Pipeline();
	Render_Pipeline() = default;
	Render_Pipeline(const Render_Pipeline&) = delete;
	Render_Pipeline &operator = (const Render_Pipeline&) = delete;

	void bind(VkCommandBuffer command_buffer); 

	static void default_pipeline_config(pipeline_config_info& config_info);
	static void enable_alpha_blending(pipeline_config_info& config_info);

private:
	static std::vector<char> read_file(const std::string& file_path);
	void create_render_pipeline(const std::string& vert_path, const std::string& frag_path, const pipeline_config_info& config);
	void create_shader_module(const std::vector<char>& shader_code, VkShaderModule* shader_module);


	My_Engine_Device& ce_device;
	VkPipeline graphics_pipeline;
	VkShaderModule vert_shader_module;
	VkShaderModule frag_shader_module;
};