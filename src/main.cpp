#include "main.h"
#include "systems\simple_render_system.h"
#include "systems\point_light_system.h"
#include "systems\transparency_system.h"
#include "rendering\ce_camera.h"
#include "keyboard_movement_controller.h"
#include "rendering\ce_buffer.h"
#include "world_gen\svo.h"
#include "systems/voxel_render_system.h"
#include "rendering/ce_frame_info.h"

#include "world_gen/ce_chunk.h"
#include "entities/voxel_components.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>
#include<glm/gtc/constants.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <STB_Image/stb_image.h>

//std
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <array>

int main() {
    Program program = {};
    try {
        program.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what()<<"\n";
        return EXIT_FAILURE;
    };
    return EXIT_SUCCESS;
}

void Program::run() {
    std::vector<std::unique_ptr<CE_Buffer>> ubo_buffers(MyEngineSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i{ 0 }; i < ubo_buffers.size(); i++) {
        ubo_buffers[i] = std::make_unique<CE_Buffer>(
            ce_device,
            sizeof(Global_UBO),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT /*| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT */
            );
        ubo_buffers[i]->map();
    }
    
    auto global_set_layout = CE_Descriptor_Set_Layout::Builder(ce_device)
        .add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
        .build();

    
    std::vector<std::unique_ptr<CE_Buffer>> tex_buffers(MyEngineSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i{ 0 }; i < tex_buffers.size(); i++) {
        tex_buffers[i] = std::make_unique<CE_Buffer>(
            ce_device,
            sizeof(CE_Image::VK_Texture),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 
            );
        tex_buffers[i]->map();
    }
    

    auto tex_set_layout = CE_Descriptor_Set_Layout::Builder(ce_device)
        .add_binding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
        .build();

    auto tex_desc_pool = CE_Descriptor_Pool::Builder(ce_device)
    .add_pool_size(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,static_cast<uint32_t>(MyEngineSwapChain::MAX_FRAMES_IN_FLIGHT))
    .build();
    //descriptor sets for textures specifically

    CE_Image::VK_Texture tex(ce_device);

    std::vector<VkDescriptorSet> global_descriptor_sets(MyEngineSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i{ 0 }; i < global_descriptor_sets.size(); i++) {
        auto buffer_info = ubo_buffers[i]->descriptorInfo();
        CE_Descriptor_Writer(*global_set_layout, *global_pool)
            .write_buffer(0, &buffer_info)
            .write_image(1,&tex.get_image_info()) //almost there
            .build(global_descriptor_sets[i]);

       
  
    }

    global_coordinator.Init();
    Simple_Render_System simple_render_system{ce_device,ce_renderer.get_swapchain_render_pass(), global_set_layout->get_descriptor_set_layout()};
    Point_Light_System point_light_system{ce_device,ce_renderer.get_swapchain_render_pass(), global_set_layout->get_descriptor_set_layout()};
    Transparency_System transperncy_system{ce_device,ce_renderer.get_swapchain_render_pass(), global_set_layout->get_descriptor_set_layout()};
    global_coordinator.Init();

    CE_Camera camera{};
   
    camera.set_view_target(glm::vec3(-1.0f,-2.0f,2.f), glm::vec3(0.0f,0.0f,2.5f));

    auto viewer_object = CE_Game_Object::create_game_object();
    viewer_object.transform_3d.translation = { 0.0f,0.0f,-2.5f };
    Keyboard_Movement_Controller camera_controller{};

    auto current_time = std::chrono::high_resolution_clock::now();
    
    //(pos,half_size)
    // AABB octree_bounds {{0,0,0},{16,16,16}};
    // AABB child_bounds {{16,8,1},{1,1,1}};
    // SVO octree {octree_bounds,16};
    // octree.insert(child_bounds,0);
    // octree.insert({{14,8,1},{1,1,1}},0);
    // std::vector<AABB> result;
    // octree.query_range({{0,0,0},{1,1,1}},result);
    

    // SVO octree_c = {octree_bounds,64};
    // CE_Chunk chunk {{0,0,0}};
    // chunk.generate_chunk({0,0,0},&octree_c);
    
    // for(AABB &bound : result) {
    //     std::cout<< "(" << bound.position.x <<"," <<bound.position.y <<","<< bound.position.z <<")"<< std::endl;
    // }

    
    // global_coordinator.register_voxel_component<Voxel_Base>();
    // auto render_vox = global_coordinator.register_system<Voxel_Render_System>(ce_device);
    // render_vox->initialize_renderer(ce_renderer.get_swapchain_render_pass(), global_set_layout->get_descriptor_set_layout());
    // signature sig;
    // sig.set(global_coordinator.voxel_get_component_type<Voxel_Base>());
    // global_coordinator.voxel_set_system_signature<Voxel_Render_System>(sig);
   
    // load_entites();

    //need to troubleshoot issues
    

    while (!ce_window.should_close()) {

        glfwPollEvents();

        auto new_time = std::chrono::high_resolution_clock::now();
        float frame_time = std::chrono::duration<float, std::chrono::seconds::period>(new_time - current_time).count();
        current_time = new_time;

        camera_controller.move_in_plane_XZ(ce_window.get_window(),frame_time,viewer_object);
        camera.set_view_YXZ(viewer_object.transform_3d.translation, viewer_object.transform_3d.rotation);

        float aspect_ratio = ce_renderer.get_aspect_ratio();
       
        camera.set_perspective_projection(glm::radians(50.0f), aspect_ratio,0.1f,10.0f);




        if (auto command_buffer = ce_renderer.begin_frame()) {

            
            int frame_index = ce_renderer.get_frame_index();
            CE_Frame_Info frame_info{
                frame_index,
                frame_time,
                command_buffer,
                camera,
                global_descriptor_sets[frame_index],
                game_objects,
                used_voxels
            };
            //info = frame_info;
            //------
            //UPDATE
            //------
            Global_UBO ubo{};
            ubo.projection_matrix = camera.get_projection();
            ubo.view_matrix = camera.get_view();
            ubo.inverse_view_matrix = camera.get_inverse_view();
            point_light_system.update(frame_info,ubo);
            ubo_buffers[frame_index]->writeToBuffer(&ubo);
            ubo_buffers[frame_index]->flush();
            //------
            //RENDER
            //------
            //possible to add other render passes now

            ce_renderer.begin_swapchain_render_pass(command_buffer);
            simple_render_system.render_game_objects(frame_info);
            //render_vox->render_voxels(frame_info);
            
            point_light_system.render(frame_info);
            transperncy_system.render_transparent_objects(frame_info);
            ce_renderer.end_swapchain_render_pass(command_buffer);
            ce_renderer.end_frame();
        }
    }
    vkDeviceWaitIdle(ce_device.device());
    //render_vox->free_memory();
    //ce_window.~Crec_Window();        
}

Program::Program() {
    global_pool = CE_Descriptor_Pool::Builder(ce_device)
        .set_max_sets(MyEngineSwapChain::MAX_FRAMES_IN_FLIGHT)
        .add_pool_size(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MyEngineSwapChain::MAX_FRAMES_IN_FLIGHT)
        .build();
    load_game_objects();
}

Program::~Program() {
    
}


void Program::load_game_objects() {
   

    std::shared_ptr<CE_Model> ce_model = CE_Model::create_model_from_file(ce_device, "models/smooth_vase.obj");  
    auto vase = CE_Game_Object::create_game_object();
    vase.model = ce_model;
    vase.transform_3d.translation = { 0.5f,0.5f,0.0f };
    vase.transform_3d.scale = {3.0f,1.5f,3.0f};
    game_objects.emplace(vase.get_id(),std::move(vase));

    ce_model = CE_Model::create_model_from_file(ce_device, "models/flat_vase.obj");
    auto flat_vase = CE_Game_Object::create_game_object();
    flat_vase.model = ce_model;
    flat_vase.transform_3d.translation = { -0.5f,0.5f,0.0f };
    flat_vase.transform_3d.scale = { 3.0f,3.0f,3.0f };
    game_objects.emplace(flat_vase.get_id(),std::move(flat_vase));

   ce_model = CE_Model::create_model_from_file(ce_device, "models/quad.obj");
    auto quad = CE_Game_Object::create_game_object();
    quad.model = ce_model;
    quad.transform_3d.translation = { 0.0f,0.5f,0.0f };
    quad.transform_3d.scale = { 3.0f,1.0f,3.0f };
    game_objects.emplace(quad.get_id(),std::move(quad));

     std::vector<glm::vec3> light_colors{
      {1.f, .1f, .1f},
      {.1f, .1f, 1.f},
      {.1f, 1.f, .1f},
      {1.f, 1.f, .1f},
      {.1f, 1.f, 1.f},
      {1.f, 1.f, 1.f}  //
  };
  
  for(int i =0;i<light_colors.size();i++) {
    auto point_light = CE_Game_Object::create_point_light(0.2f);
    point_light.color = light_colors[i];
    auto rotate = glm::rotate(glm::mat4(1.0f), (i * glm::two_pi<float>())/light_colors.size(),{0.0f,-1.0f,0.0f});
    point_light.transform_3d.translation = glm::vec3(rotate*glm::vec4(-1.0f,-1.0f,-1.0f,1.0f));
    game_objects.emplace(point_light.get_id(),std::move(point_light));
  }

    ce_model = CE_Model::create_model_from_file(ce_device, "models/pyramid.obj");
    auto trandparent = CE_Game_Object::create_transparent_object(0.5f);
    trandparent.model = ce_model;
    trandparent.transform_3d.translation = glm::vec3(-1.0f,1.0f,1.0f);
    trandparent.transform_3d.scale = { 1.0f,1.0f,1.0f };
    game_objects.emplace(trandparent.get_id(), std::move(trandparent));

    
    //materials
    /*
    std::unordered_map<std::shared_ptr<CE_Model>, const char*> file_names = {
        {game_objects[0].model,"textures/TT bricks.png"},
        {game_objects[1].model,"textures/TT Dirt.png"},
        {game_objects[2].model,"textures/TT bricks.png"},
        {game_objects[3].model,"textures/TT push stone.png"}
    };*/
    
    //dexcriptor pool

    //textures
    //texture_info.command_buffer = info.command_buffer;
    //texture_info.queue = queue
    //texture_info.device = ce_device.device();
    //texture_info.layout = layout->get_descriptor_set_layout();
    //texture_info.descriptor_pool = ...

    //for (const auto& [obj, file_name] :file_names) {
       // texture_info.file_name = file_name;
       // materials[obj] = new CE_Image::Texture(texture_info);
    //}
    
    
   // CE_Image::Texture tex {ce_device};
}

void Program::load_entites() {
    std::shared_ptr<CE_Model> ce_model = CE_Model::create_model_from_file(ce_device, "models/pyramid.obj");
    auto vox = global_coordinator.create_voxel();
   
    
    global_coordinator.voxel_add_component(
        vox,
        Voxel_Base {IS_FLAMMABLE | IS_ROTATABLE, ce_model, {{1.0f,-4.0f,1.0f},{0.5f,0.5f,0.5f}}}
    );
    auto vox_base = global_coordinator.voxel_get_component<Voxel_Base>(vox);
    vox_base.model = ce_model;
    vox_base.transform_3d.scale = {0.5f,0.5f,0.5f};
    vox_base.transform_3d.translation = {1.0f,-4.0f,1.0f};
    used_voxels.push_back(vox);
}