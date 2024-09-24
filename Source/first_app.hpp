#pragma once
#include "lve_window.hpp"
#include "lve_game_object.hpp"
#include "lve_pipeline.hpp"
#include "lve_swap_chain.hpp"
#include <memory>
#include <vector>

namespace lve {
	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();
		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;

		void run();
	private:
		LveWindow lveWindow{ WIDTH, HEIGHT, "Vulkan" };
		LveDevice lveDevice{ lveWindow };
		std::unique_ptr <LveSwapChain> lveSwapChain;
		std::unique_ptr<LvePipeline> lvePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<LveGameObject> gameObjects{};
		
		void LoadGameObjects();
		void CreatePipelineLayout();
		void CreatePipeline();
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void DrawFrame();
		void RecreateSwapChain();
		void RecordCommandBuffer(int imageIndex);
		void RenderGameObjects(VkCommandBuffer commandBuffer);
	};
}	// namespace lve