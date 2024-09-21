#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace lve {
	class LveWindow {
	public:
		LveWindow(int w, int h, std::string name);
		~LveWindow();

		LveWindow(const LveWindow&) = delete;
		LveWindow& operator*(const LveWindow&) = delete;

		[[nodiscard]] inline bool shouldClose() { return glfwWindowShouldClose(window); }
	private: 
		GLFWwindow* window;
	};
}	// namespace lve