#include "lve_window.hpp"
#include <stdexcept>

namespace lve {
	LveWindow::LveWindow(int w, int h, std::string name) 
	: width(static_cast<uint32_t>(w)), height(static_cast<uint32_t>(h)) {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(w, h, name.c_str(), nullptr, nullptr);
	}

	LveWindow::~LveWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void LveWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Window Surface.");
		}
	}
}