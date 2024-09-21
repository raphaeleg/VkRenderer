#pragma once
#include "lve_window.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"

namespace lve {
	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();
	private:
		LveWindow lveWindow{ WIDTH, HEIGHT, "Vulkan" };
		LveDevice lveDevice{ lveWindow };
		LvePipeline lvePipeline{
			lveDevice, 
			"Source/shaders/simple_shader.vert.spv", 
			"Source/shaders/simple_shader.frag.spv",
			LvePipeline::DefautlPipelineConfigInfo(WIDTH, HEIGHT)
		};
	};
}	// namespace lve