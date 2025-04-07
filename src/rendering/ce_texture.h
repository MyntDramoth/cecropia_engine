#pragma once


#include <STB_Image/stb_image.h>

#include "../types.h"
#include "my_engine_device.h"
#include "ce_buffer.h"



#include <stdexcept>
#include <vulkan/vulkan.h>

namespace CE_Image {


   

    typedef struct sprite_sheet {
    f32 width;
    f32 height;
    f32 cell_width;
    f32 cell_height;
    u32 texture_id;
    }Sprite_Sheet;

    class VK_Texture { 
        public:
            VK_Texture(My_Engine_Device& ce_device, const char* file_path = "../textures/DEFAULT_TEXTURE.jpg");
            ~VK_Texture();
            void use_texture(VkCommandBuffer buffer, VkPipelineLayout pipeline_layout);
            VkDescriptorImageInfo get_image_info () { tex_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; tex_info.imageView = image_view; tex_info.sampler = sampler; return tex_info;}
            VkImage get_image() {return image;}
        private:
            u32 id;
            
            VkDevice device;
            const char* file_path_name;
            const char* missing_texture_path {"../textures/MISSING_TEXTURE.jpg"};
    
            //resources
            VkImage image;
            VkDeviceMemory image_memory;
            VkImageView image_view;
            VkSampler sampler;

            VkDescriptorImageInfo tex_info{};
    

            //resource descriptors
            VkDescriptorSetLayout layout;
            VkDescriptorSet descriptor_set;
            VkDescriptorPool descriptor_pool;

            VkCommandBuffer command_buffer;
            VkCommandPool command_pool;
            VkQueue graphics_queue;
            My_Engine_Device& ce_device;

            //sets
             VkImageCreateInfo image_info{};

            void load_texture();
            void make_image_info(int tex_width, int tex_height);
            void create_image(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& in_image, VkDeviceMemory& image_memory);


            void populate();
            void make_sampler();

            void make_texture_view();
            //descriptor set making --- already done elsewhere
            void make_descriptor_set();

            //helpers
            VkCommandBuffer begin_single_time_commands();
            void end_single_time_commands(VkCommandBuffer command_buffer);
            
            void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
            void copy_buffer_to_image(VkBuffer buffer, VkImage in_image, uint32_t width, uint32_t height);
            void transition_image_layout(VkImage in_image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);

            VkImageView create_image_view(VkImage in_image, VkFormat format);
            void create_sampler();
    };

}