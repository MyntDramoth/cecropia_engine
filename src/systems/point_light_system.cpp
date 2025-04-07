#include "point_light_system.h"


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


struct Point_Light_Push_Constants {
    glm::vec4 position {}; //ignore w
	glm::vec4 color {};
    float radius;
};

Point_Light_System::Point_Light_System(My_Engine_Device& device, VkRenderPass render_pass, VkDescriptorSetLayout global_descriptor_set_layout) : ce_device{device} {
   
    create_pipeline_layout(global_descriptor_set_layout);
    create_pipeline(render_pass);
}

Point_Light_System::~Point_Light_System() {
    vkDestroyPipelineLayout(ce_device.device(), pipeline_layout, nullptr);
}

void Point_Light_System::create_pipeline_layout(VkDescriptorSetLayout global_descriptor_set_layout) {

    VkPushConstantRange push_constant_range{};
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = sizeof(Point_Light_Push_Constants);

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

void Point_Light_System::create_pipeline(VkRenderPass render_pass) {

    assert(pipeline_layout != nullptr && "Cannot create pipeline before pipeline layout");

    pipeline_config_info pipline_config{};
    Render_Pipeline::default_pipeline_config(pipline_config);
    pipline_config.binding_descriptions.clear();
    pipline_config.attribute_descriptions.clear();
    pipline_config.render_pass = render_pass;
    pipline_config.pipeline_layout = pipeline_layout;
    render_pipeline = std::make_unique<Render_Pipeline>(ce_device, "shaders/point_light_shader.vert.spv", "shaders/point_light_shader.frag.spv", pipline_config);
}

void Point_Light_System::render(CE_Frame_Info& frame_info) {
    render_pipeline->bind(frame_info.command_buffer);
   
    vkCmdBindDescriptorSets(frame_info.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, 1, &frame_info.global_descriptor_set, 0, nullptr);
    for(auto& kv : frame_info.game_objects) {
        auto &obj = kv.second;
        if(obj.point_light == nullptr) {continue;}
        Point_Light_Push_Constants push{};
        push.position = glm::vec4(obj.transform_3d.translation, 1.0f);
        push.color = glm::vec4(obj.color,obj.point_light->light_intensity);
        push.radius = obj.transform_3d.scale.x;
        vkCmdPushConstants(frame_info.command_buffer,pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,0,sizeof(Point_Light_Push_Constants),&push);
        vkCmdDraw(frame_info.command_buffer,6,1,0,0);
    }
   
}

void Point_Light_System::update(CE_Frame_Info& frame_info,Global_UBO &ubo) {
    int light_index = 0;
    for(auto& kv : frame_info.game_objects) {
        auto &obj = kv.second;
        if(obj.point_light == nullptr) {continue;}
        //copy light to ubo
        ubo.point_lights[light_index].position = glm::vec4(obj.transform_3d.translation, 1.0f);
        ubo.point_lights[light_index].color = glm::vec4(obj.color,obj.point_light->light_intensity);
        light_index +=1;
    }
    ubo.num_lights = light_index;
}
