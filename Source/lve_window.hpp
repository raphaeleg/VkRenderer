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
		[[nodiscard]] inline bool isWindowResized() { return frameBufferResized; }
		[[nodiscard]] inline void ResetWindowResizedFlag() { frameBufferResized = false; }
		VkExtent2D GetExtent() { return { width, height }; }
		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
	private: 
		GLFWwindow* window;
		uint32_t width;
		uint32_t height;
		bool frameBufferResized = false;

		static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);
	};
}	// namespace lve