#include "transparency_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>
#include<glm/gtc/constants.hpp>

//std
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <array>
#include <map>

struct simple_push_constant_data_3d {
    glm::mat4 model_matrix{ 1.0f };
    glm::mat4 normal_matrix{ 1.0f };
    //alignas(16) glm::vec3 color;
};

Transparency_System::Transparency_System(My_Engine_Device& device, VkRenderPass render_pass, VkDescriptorSetLayout global_descriptor_set_layout) : ce_device{device} {
    create_pipeline_layout(global_descriptor_set_layout);
    create_pipeline(render_pass);
}

Transparency_System::~Transparency_System() {
    vkDestroyPipelineLayout(ce_device.device(), pipeline_layout, nullptr);
}

void Transparency_System::create_pipeline_layout(VkDescriptorSetLayout global_descriptor_set_layout) {

    VkPushConstantRange push_constant_range{};
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = sizeof(simple_push_constant_data_3d);

    std::vector<VkDescriptorSetLayout> descriptor_set_layout{ global_descriptor_set_layout };

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

void Transparency_System::create_pipeline(VkRenderPass render_pass) {

    assert(pipeline_layout != nullptr && "Cannot create pipeline before pipeline layout");

    pipeline_config_info pipline_config{};
    Render_Pipeline::default_pipeline_config(pipline_config);
    Render_Pipeline::enable_alpha_blending(pipline_config);

    pipline_config.render_pass = render_pass;
    pipline_config.pipeline_layout = pipeline_layout;
    render_pipeline = std::make_unique<Render_Pipeline>(ce_device, "shaders/default.vert.spv", "shaders/default.frag.spv", pipline_config);
}

void Transparency_System::render_transparent_objects(CE_Frame_Info& frame_info) {
    //sort objects
    std::map<float,CE_Game_Object::id_t> sorted;

    for (auto& kv : frame_info.game_objects) {
            auto& obj = kv.second;
            if(obj.transparency == nullptr) {continue;}

            //calculate distance
            auto offset = frame_info.camera.get_position() - obj.transform_3d.translation;
            float dis_squared = glm::dot(offset,offset);
            sorted[dis_squared] = obj.get_id();

    }


    render_pipeline->bind(frame_info.command_buffer);
   
    vkCmdBindDescriptorSets(frame_info.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, 1, &frame_info.global_descriptor_set, 0, nullptr);


    //sort in reverse order (aka) back to front
    for (auto i = sorted.rbegin(); i != sorted.rend();++i) {
        auto& obj = frame_info.game_objects.at(i->second);
        
        simple_push_constant_data_3d push{};      
        
        push.model_matrix = obj.transform_3d.mat4();
        push.normal_matrix = obj.transform_3d.normal_matrix();

        vkCmdPushConstants(frame_info.command_buffer, pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(simple_push_constant_data_3d), &push);
        obj.model->bind(frame_info.command_buffer);
        obj.model->draw(frame_info.command_buffer);
    }
}