#pragma once
#include "crec_window.h"
#include "my_engine_device.h"
#include "my_engine_swap_chain.h"

#include <cassert>
#include <memory>
#include <vector>
#include "../types.h"

typedef struct sprite_sheet {
    f32 width;
    f32 height;
    f32 cell_width;
    f32 cell_height;
    u32 texture_id;
}Sprite_Sheet;

class CE_Renderer {
public:	

	CE_Renderer(CE_Window& window, My_Engine_Device& device);
	~CE_Renderer();

	CE_Renderer(const CE_Renderer&) = delete;
	CE_Renderer& operator=(const CE_Renderer&) = delete;

	VkRenderPass get_swapchain_render_pass() const { return swapchain->getRenderPass(); };
	float get_aspect_ratio() const { return swapchain->extentAspectRatio(); };
	bool is_frame_in_progress() const { return is_frame_started; };
	VkCommandBuffer get_current_command_buffer() const { 
		assert(is_frame_started && "cannot get command buffer when frame is not in progress");
		return command_buffers[current_frame_index];
	};

	int get_frame_index() const {
		assert(is_frame_started && "cannot get frame index when frame is not in progress");
		return current_frame_index;
	};

	VkCommandBuffer begin_frame();
	void end_frame();
	void begin_swapchain_render_pass(VkCommandBuffer command_buffer);
	void end_swapchain_render_pass(VkCommandBuffer command_buffer);
	
private:			
	void create_command_buffers();
	void free_command_buffers();
	void recreate_swapchain();
	
	CE_Window& ce_window;
	My_Engine_Device& ce_device;
	std::unique_ptr<MyEngineSwapChain> swapchain;	
	std::vector<VkCommandBuffer> command_buffers;
	
	uint32_t current_image_index{0};
	int current_frame_index{ 0 };
	bool is_frame_started{ false };
};

