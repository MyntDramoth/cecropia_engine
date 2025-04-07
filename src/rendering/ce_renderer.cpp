
#include "ce_renderer.h"

//std
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include<array>

CE_Renderer::CE_Renderer(CE_Window& window, My_Engine_Device& device) : ce_window{ window }, ce_device{device} {
   
    recreate_swapchain();
    create_command_buffers();
}

CE_Renderer::~CE_Renderer() {
    free_command_buffers();
}

void CE_Renderer::recreate_swapchain() {
    auto extent = ce_window.get_extent();
    while (extent.width == 0 || extent.height == 0) {
        extent = ce_window.get_extent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(ce_device.device());
    if (swapchain == nullptr) {
        swapchain = std::make_unique<MyEngineSwapChain>(ce_device, extent);
    }
    else {
        std::shared_ptr<MyEngineSwapChain> old_swapchain = std::move(swapchain);
        swapchain = std::make_unique<MyEngineSwapChain>(ce_device, extent, old_swapchain);

        if (!old_swapchain->compare_swap_formats(*swapchain.get())) {
            throw std::runtime_error("swapchain image format has changed!");
        }
    }
   
    //TODO we'll come back to this in a second

}

void CE_Renderer::create_command_buffers() {
    command_buffers.resize(MyEngineSwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = ce_device.getCommandPool();
    alloc_info.commandBufferCount = static_cast<uint32_t>(command_buffers.size());

    if (vkAllocateCommandBuffers(ce_device.device(), &alloc_info, command_buffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void CE_Renderer::free_command_buffers() {
    vkFreeCommandBuffers(ce_device.device(), ce_device.getCommandPool(), static_cast<uint32_t>(command_buffers.size()), command_buffers.data());
    command_buffers.clear();
}

VkCommandBuffer  CE_Renderer::begin_frame() {
    assert(!is_frame_started && "can't call begin frame when frame is already in progress!");  
    auto result = swapchain->acquireNextImage(&current_image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swapchain();
        return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to aquire swap chain image!");
    }

    is_frame_started = true;
    auto command_buffer = get_current_command_buffer();

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    return command_buffer;
}

void  CE_Renderer::end_frame() {
    assert(is_frame_started && "can't call end frame while frame is not in progress!");
    auto command_buffer = get_current_command_buffer();
    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

    auto result = swapchain->submitCommandBuffers(&command_buffer, &current_image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || ce_window.was_window_resized()) {
        ce_window.reset_window_resized_flag();
        recreate_swapchain();   
    }

    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    is_frame_started = false;
    current_frame_index = (current_frame_index+1) % MyEngineSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void  CE_Renderer::begin_swapchain_render_pass(VkCommandBuffer command_buffer) {
    assert(is_frame_started && "can't begin swaphacin render pass while frame is not in progress!");
    assert(command_buffer == get_current_command_buffer() && "can't begin render pass on a command buffer from a different frame!");
    
    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = swapchain->getRenderPass();
    render_pass_info.framebuffer = swapchain->getFrameBuffer(current_image_index);

    render_pass_info.renderArea.offset = { 0,0 };
    render_pass_info.renderArea.extent = swapchain->getSwapChainExtent();

    std::array<VkClearValue, 2> clear_values{};
    clear_values[0].color = { 0.01f,0.01f,0.01f, 1.0f };
    clear_values[1].depthStencil = { 1.0f, 0 };

    render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
    render_pass_info.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapchain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(swapchain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{ {0, 0}, swapchain->getSwapChainExtent() };
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}

void  CE_Renderer::end_swapchain_render_pass(VkCommandBuffer command_buffer) {
    assert(is_frame_started && "can't end swaphacin render pass while frame is not in progress!");
    assert(command_buffer == get_current_command_buffer() && "can't end render pass on a command buffer from a different frame!");
    
    vkCmdEndRenderPass(command_buffer);
}
