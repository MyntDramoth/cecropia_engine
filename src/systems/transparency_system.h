#pragma once
#include "../rendering/render_pipeline.h"
#include "../rendering/my_engine_device.h"
#include "ce_game_object.h"
#include "../rendering/ce_camera.h"
#include "../rendering/ce_frame_info.h"

#include <memory>
#include <vector>

class Transparency_System {
public:	

	Transparency_System(My_Engine_Device& device, VkRenderPass render_pass, VkDescriptorSetLayout global_descriptor_set_layout);
	~Transparency_System();

	Transparency_System(const Transparency_System&) = delete;
	Transparency_System& operator=(const Transparency_System&) = delete;

	void render_transparent_objects(CE_Frame_Info& frame_info);
private:
	
	void create_pipeline_layout(VkDescriptorSetLayout global_descriptor_set_layout);
	void create_pipeline(VkRenderPass render_pass);
	
	My_Engine_Device& ce_device;	

	std::unique_ptr<Render_Pipeline> render_pipeline;
	VkPipelineLayout pipeline_layout;
};