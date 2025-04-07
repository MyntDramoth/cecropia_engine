#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

class CE_Window {
public:

	CE_Window(int width, int height, std::string name);
	~CE_Window();

	CE_Window(const CE_Window&) = delete;
	CE_Window &operator=(const CE_Window&) = delete;

	VkExtent2D get_extent() { return { static_cast<uint32_t>(width),  static_cast<uint32_t>(height) }; }
	GLFWwindow* get_window() const { return window; }

	bool should_close();
	bool was_window_resized() { return frame_buffer_resized; };

	void create_window_surface(VkInstance instance, VkSurfaceKHR *surface);
	void reset_window_resized_flag() { frame_buffer_resized = false; };
	
private:
	static void frame_buffer_resized_callback(GLFWwindow* window,int width,int height);
	void init_window(void);

	
	std::string window_name;

	int width;
	int height;
	bool frame_buffer_resized = false;
	GLFWwindow* window;
};



