#pragma once
#include "../rendering/render_pipeline.h"
#include "../rendering/my_engine_device.h"
#include "ce_game_object.h"
#include "../rendering/ce_camera.h"
#include "../rendering/ce_frame_info.h"

#include <memory>
#include <vector>

class Point_Light_System {
public:	

	Point_Light_System(My_Engine_Device& device, VkRenderPass render_pass, VkDescriptorSetLayout global_descriptor_set_layout);
	~Point_Light_System();

	Point_Light_System(const Point_Light_System&) = delete;
	Point_Light_System& operator=(const Point_Light_System&) = delete;

	void update(CE_Frame_Info& frame_info,Global_UBO &ubo);
	void render(CE_Frame_Info& frame_info);
private:
	
	void create_pipeline_layout(VkDescriptorSetLayout global_descriptor_set_layout);
	void create_pipeline(VkRenderPass render_pass);
	
	My_Engine_Device& ce_device;	

	std::unique_ptr<Render_Pipeline> render_pipeline;
	VkPipelineLayout pipeline_layout;
};

