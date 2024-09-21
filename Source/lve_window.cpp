#include "lve_window.hpp"

namespace lve {
	LveWindow::LveWindow(int w, int h, std::string name) {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(w, h, name.c_str(), nullptr, nullptr);
	}

	LveWindow::~LveWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}
}