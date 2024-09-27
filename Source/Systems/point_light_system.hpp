#pragma once
#include "../lve_game_object.hpp"
#include "../lve_camera.hpp"
#include "../lve_pipeline.hpp"
#include "../lve_frame_info.hpp"
#include <memory>
#include <vector>

namespace lve {
	class PointLightSystem {
	public:
		PointLightSystem(LveDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();
		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void Render(FrameInfo &frameInfo);

	private:
		LveDevice &lveDevice ;
		std::unique_ptr<LvePipeline> lvePipeline;
		VkPipelineLayout pipelineLayout;
		
		void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void CreatePipeline(VkRenderPass renderPass);
	};
}	// namespace lve