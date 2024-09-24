#pragma once
#include "lve_window.hpp"
#include "lve_swap_chain.hpp"
#include <memory>
#include <vector>
#include <cassert>

namespace lve {
	class LveRenderer {
	public:
		LveRenderer(LveWindow& window, LveDevice& device);
		~LveRenderer() { FreeCommandBuffers(); }
		LveRenderer(const LveRenderer&) = delete;
		LveRenderer& operator=(const LveRenderer&) = delete;

		VkCommandBuffer BeginFrame();
		void EndFrame();
		void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer) const;

		VkRenderPass GetSwapChainRenderPass() const { return lveSwapChain->getRenderPass(); }
		bool IsFrameInProgress() const { return isFrameStarted; }
		VkCommandBuffer GetCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame is not in progress");
			return commandBuffers[currentFrameIndex];
		}
		int GetFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame is not in progress");
			return currentFrameIndex;
		}

	private:
		LveWindow& lveWindow;
		LveDevice& lveDevice;
		std::unique_ptr<LveSwapChain> lveSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;
		uint32_t currentImageIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{ false };

		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapChain();
	};
}	// namespace lve