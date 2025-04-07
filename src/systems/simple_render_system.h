#pragma once
#include "../rendering/render_pipeline.h"
#include "../rendering/my_engine_device.h"
#include "ce_game_object.h"
#include "../rendering/ce_camera.h"
#include "../rendering/ce_frame_info.h"
#include "../rendering/ce_texture.h"
#include "../entities/ce_coordinator.h"
#include "../entities/voxel_components.h"
#include "voxel_render_system.h"

#include <memory>
#include <vector>

extern Coordinator global_coordinator;

class Simple_Render_System: public System {
public:	

	Simple_Render_System(My_Engine_Device& device, VkRenderPass render_pass, VkDescriptorSetLayout global_descriptor_set_layout);
	~Simple_Render_System();

	Simple_Render_System(const Simple_Render_System&) = delete;
	Simple_Render_System& operator=(const Simple_Render_System&) = delete;

	void render_game_objects(CE_Frame_Info& frame_info);
	void render_voxels(CE_Frame_Info& frame_info);
	
private:
	
	void create_pipeline_layout(VkDescriptorSetLayout global_descriptor_set_layout);
	void create_pipeline(VkRenderPass render_pass);
	
	My_Engine_Device& ce_device;
	//std::unordered_map<std::shared_ptr<CE_Model>, CE_Image::Texture*> materials;
	std::unique_ptr<Render_Pipeline> render_pipeline;
	VkPipelineLayout pipeline_layout;
};