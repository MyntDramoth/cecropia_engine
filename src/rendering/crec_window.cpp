#include "crec_window.h"
#include <stdexcept>

CE_Window::CE_Window(int width, int height, std::string name) : width{ width }, height{ height }, window_name{name} {
	init_window();
	//the last perameter in create window is open GL specific
	
}

CE_Window::~CE_Window() {
	glfwDestroyWindow(window);
	glfwTerminate();

}

void CE_Window::init_window(void) {
	glfwInit();
	//these two parts tell the window not to open an open GL environment
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(width, height, window_name.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window,this);
	glfwSetFramebufferSizeCallback(window, frame_buffer_resized_callback);
}

void CE_Window::create_window_surface(VkInstance instance, VkSurfaceKHR *surface) {
	
	if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface");
	}
}

bool CE_Window::should_close() {
	return glfwWindowShouldClose(window);
}

void CE_Window::frame_buffer_resized_callback(GLFWwindow* window, int width, int height) {
	auto ce_window = reinterpret_cast<CE_Window*>(glfwGetWindowUserPointer(window));
	ce_window->frame_buffer_resized = true;
	ce_window->width = width;
	ce_window->height = height;
}