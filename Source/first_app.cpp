#include "first_app.hpp"
#include <stdexcept>
#include <array>

namespace lve{
    FirstApp::FirstApp() {
        LoadModels();
        CreatePipelineLayout();
        CreatePipeline();
        CreateCommandBuffers();
    }

    FirstApp::~FirstApp() {
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
        vkFreeCommandBuffers(
            lveDevice.device(),
            lveDevice.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data());
        commandBuffers.clear();
    }

	void FirstApp::run() {
		while (!lveWindow.shouldClose()) {
			glfwPollEvents();
            DrawFrame();
		}
        vkDeviceWaitIdle(lveDevice.device());
	}

    void FirstApp::CreatePipelineLayout() {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void FirstApp::CreatePipeline() {
        auto pipelineConfig =
            LvePipeline::DefaultPipelineConfigInfo(lveSwapChain.width(), lveSwapChain.height());
        pipelineConfig.renderPass = lveSwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>(
            lveDevice,
            "Source/shaders/simple_shader.vert.spv",
            "Source/shaders/simple_shader.frag.spv",
            pipelineConfig);
    }

    void FirstApp::CreateCommandBuffers() {
        commandBuffers.resize(lveSwapChain.imageCount());
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = lveDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        // Record to Command Buffers
        for (size_t i = 0; i < commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0;                   // Optional
            beginInfo.pInheritanceInfo = nullptr;  // Optional

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }
            VkRenderPassBeginInfo renderPassInfo = {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = lveSwapChain.getRenderPass();
            renderPassInfo.framebuffer = lveSwapChain.getFrameBuffer(i);

            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = lveSwapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };  // clear with a black grey background
            clearValues[1].depthStencil = { 1.0f, 0 };  // this was how we defined the attachments in swapchain
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            // inline = no secondary buffers // SECONDARY_COMMAND_BUFFERS
            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            lvePipeline->Bind(commandBuffers[i]);
            lveModel->Bind(commandBuffers[i]);
            lveModel->Draw(commandBuffers[i]);

            vkCmdEndRenderPass(commandBuffers[i]);
            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }

    void FirstApp::DrawFrame() {
        uint32_t imageIndex;
        auto result = lveSwapChain.acquireNextImage(&imageIndex);
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image");
        }

        result = lveSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }

    void FirstApp::LoadModels() {
        std::vector<LveModel::Vertex> base{
          {{ 0.0, -0.9 }},
          { {0.9, 0.9 }},
          {{ -0.9, 0.9 }}
        };
        std::vector<LveModel::Vertex> vertices = SierpinskiTriangle(8, base);

        lveModel = std::make_unique<LveModel>(lveDevice, vertices);
    }

    std::vector<LveModel::Vertex> FirstApp::SierpinskiTriangle(int depth, std::vector<LveModel::Vertex> v) {
        if (depth < 1) {
            return v;
        }
        // find the halfway point
        auto a = v[0].position;
        auto b = v[1].position;
        auto c = v[2].position;
        glm::vec2 ab = { (a.x + b.x) / 2, (a.y + b.y) / 2 };
        glm::vec2 bc = { (b.x + c.x) / 2, (b.y + c.y) / 2 };
        glm::vec2 ac = { (a.x + c.x) / 2, (a.y + c.y) / 2 };
        auto d = depth - 1;

        auto res = SierpinskiTriangle(d, { {a}, {ac}, {ab} });
        auto res1 = SierpinskiTriangle(d, { {ab}, {b}, {bc} });
        auto res2 = SierpinskiTriangle(d, { {ac}, {bc}, {c} });

        res1.insert(res1.end(), res2.begin(), res2.end());
        res.insert(res.end(), res1.begin(), res1.end());
        return res;
    }
}	// namespace lve