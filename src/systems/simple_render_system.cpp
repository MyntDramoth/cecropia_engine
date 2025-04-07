#include "simple_render_system.h"


//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>
#include<glm/gtc/constants.hpp>

//std
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include<array>

extern Coordinator global_coordinator;

struct simple_push_constant_data_2d {
    glm::mat2 transform{ 1.0f };
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
};

struct simple_push_constant_data_3d {
    glm::mat4 model_matrix{ 1.0f };
    glm::mat4 normal_matrix{ 1.0f };
    //alignas(16) glm::vec3 color;
};


Simple_Render_System::Simple_Render_System(My_Engine_Device& device, VkRenderPass render_pass, VkDescriptorSetLayout global_descriptor_set_layout) : ce_device{device} {
   
    create_pipeline_layout(global_descriptor_set_layout);
    create_pipeline(render_pass);
}

Simple_Render_System::~Simple_Render_System() {
    vkDestroyPipelineLayout(ce_device.device(), pipeline_layout, nullptr);
    /*for (const auto& [key,texture] : materials) {
        delete texture;
    };*/
}

void Simple_Render_System::create_pipeline_layout(VkDescriptorSetLayout global_descriptor_set_layout) {

    VkPushConstantRange push_constant_range{};
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = sizeof(simple_push_constant_data_3d);

    //good to go just need to pass in the texture descriptor set in

    std::vector<VkDescriptorSetLayout> descriptor_set_layout{ global_descriptor_set_layout /*,texture_descriptor_set_layout*/};

    VkPipelineLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.setLayoutCount = static_cast<uint32_t>(descriptor_set_layout.size()); 
    layout_info.pSetLayouts = descriptor_set_layout.data();
    layout_info.pushConstantRangeCount = 1;
    layout_info.pPushConstantRanges = &push_constant_range;
    if (vkCreatePipelineLayout(ce_device.device(), &layout_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipleine layout");
    }

}

void Simple_Render_System::create_pipeline(VkRenderPass render_pass) {

    assert(pipeline_layout != nullptr && "Cannot create pipeline before pipeline layout");

    pipeline_config_info pipline_config{};
    Render_Pipeline::default_pipeline_config(pipline_config);

    pipline_config.render_pass = render_pass;
    pipline_config.pipeline_layout = pipeline_layout;
    render_pipeline = std::make_unique<Render_Pipeline>(ce_device, "shaders/default.vert.spv", "shaders/default.frag.spv", pipline_config);
}


void Simple_Render_System::render_game_objects(CE_Frame_Info& frame_info) {
    render_pipeline->bind(frame_info.command_buffer);
   
    vkCmdBindDescriptorSets(frame_info.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, 1, &frame_info.global_descriptor_set, 0, nullptr);

    for (auto& kv : frame_info.game_objects) {
        auto& obj = kv.second;
        if (obj.model == nullptr) { continue; }
        simple_push_constant_data_3d push{};      
        
        push.model_matrix = obj.transform_3d.mat4();
        push.normal_matrix = obj.transform_3d.normal_matrix();

        vkCmdPushConstants(frame_info.command_buffer, pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(simple_push_constant_data_3d), &push);
        
        //materials[obj.model]->use_texture(frame_info.command_buffer,pipeline_layout);
        obj.model->bind(frame_info.command_buffer);
        obj.model->draw(frame_info.command_buffer);
    }

}

void Simple_Render_System::render_voxels(CE_Frame_Info& frame_info) {
    render_pipeline->bind(frame_info.command_buffer);
   
    vkCmdBindDescriptorSets(frame_info.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, 1, &frame_info.global_descriptor_set, 0, nullptr);

    for (auto& kv : voxels) {
        auto& obj = global_coordinator.voxel_get_component<Voxel_Base>(kv);

        if (obj.model == nullptr) { continue; }

        simple_push_constant_data_3d push{};      
        
        push.model_matrix = obj.transform_3d.mat4();
        push.normal_matrix = obj.transform_3d.normal_matrix();

        vkCmdPushConstants(frame_info.command_buffer, pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(simple_push_constant_data_3d), &push);
        //materials[obj.model]->use_texture(frame_info.command_buffer,pipeline_layout);
        obj.model->bind(frame_info.command_buffer);
        obj.model->draw(frame_info.command_buffer);
    }
}