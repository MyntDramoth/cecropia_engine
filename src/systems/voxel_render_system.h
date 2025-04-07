#pragma once

#include "../rendering/render_pipeline.h"
#include "../rendering/my_engine_device.h"
#include "../rendering/ce_camera.h"
#include "../rendering/ce_frame_info.h"
#include "../entities/voxel_components.h"
#include "simple_render_system.h"

#include <memory>
#include <vector>



class Voxel_Render_System : public System {
    public:
        Voxel_Render_System(My_Engine_Device& device);
        void initialize_renderer(VkRenderPass render_pass, VkDescriptorSetLayout global_descriptor_set_layout);
        void render_voxels(CE_Frame_Info& frame_info);
        void free_memory();
    private:
         void create_pipeline_layout(VkDescriptorSetLayout global_descriptor_set_layout);
	     void create_pipeline(VkRenderPass render_pass);
	
	    My_Engine_Device& ce_device;	

	    std::unique_ptr<Render_Pipeline> render_pipeline;
	    VkPipelineLayout pipeline_layout;
    
};