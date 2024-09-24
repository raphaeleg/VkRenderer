#include "first_app.hpp"
#include <stdexcept>
#include <array>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace lve{

    struct SimplePushConstantData {
        glm::mat2 transform{ 1.0f };
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    FirstApp::FirstApp() {
        LoadGameObjects();
        CreatePipelineLayout();
        RecreateSwapChain();
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
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void FirstApp::CreatePipeline() {
        assert(lveSwapChain != nullptr && "Cannot create pipeline before swap chain.");
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout.");

        PipelineConfigInfo pipelineConfig{};
        LvePipeline::DefaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = lveSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>(
            lveDevice,
            "Source/shaders/simple_shader.vert.spv",
            "Source/shaders/simple_shader.frag.spv",
            pipelineConfig);
    }

    void FirstApp::RecreateSwapChain() {
        auto extent = lveWindow.GetExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = lveWindow.GetExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(lveDevice.device());

        if (lveSwapChain == nullptr) {
            lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent);
        } else {
            lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent, std::move(lveSwapChain));
            if (lveSwapChain->imageCount() != commandBuffers.size()) {
                FreeCommandBuffers();
                CreateCommandBuffers();
            }
        }

        // TODO: check if render passes are compatible
        CreatePipeline();
    }

    void FirstApp::CreateCommandBuffers() {
        commandBuffers.resize(lveSwapChain->imageCount());
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = lveDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void FirstApp::FreeCommandBuffers() {
        vkFreeCommandBuffers(
            lveDevice.device(),
            lveDevice.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data());
        commandBuffers.clear();
    }

    void FirstApp::RecordCommandBuffer(int imageIndex) {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;                   // Optional
        beginInfo.pInheritanceInfo = nullptr;  // Optional

        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = lveSwapChain->getRenderPass();
        renderPassInfo.framebuffer = lveSwapChain->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = lveSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };  // clear with a black grey background
        clearValues[1].depthStencil = { 1.0f, 0 };  // this was how we defined the attachments in swapchain
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        // inline = no secondary buffers // SECONDARY_COMMAND_BUFFERS
        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(lveSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(lveSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{ {0, 0}, lveSwapChain->getSwapChainExtent() };
        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        lvePipeline->Bind(commandBuffers[imageIndex]);
        RenderGameObjects(commandBuffers[imageIndex]);

        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void FirstApp::RenderGameObjects(VkCommandBuffer commandBuffer) {
        // update
        int i = 0;
        for (auto& obj : gameObjects) {
            i += 1;
            obj.transform2d.rotation =
                glm::mod<float>(obj.transform2d.rotation + 0.0005f * i, 2.f * glm::pi<float>());
        }

        // render
        lvePipeline->Bind(commandBuffer);
        for (auto& obj : gameObjects) {
            SimplePushConstantData push{};
            push.offset = obj.transform2d.translation;
            push.color = obj.color;
            push.transform = obj.transform2d.mat2();

            vkCmdPushConstants(
                commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);
            obj.model->Bind(commandBuffer);
            obj.model->Draw(commandBuffer);
        }
    }

    void FirstApp::DrawFrame() {
        uint32_t imageIndex;
        auto result = lveSwapChain->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            RecreateSwapChain();
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image");
        }

        RecordCommandBuffer(imageIndex);
        result = lveSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lveWindow.isWindowResized()) {
            lveWindow.ResetWindowResizedFlag();
            RecreateSwapChain();
            return;
        }
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }

    void FirstApp::LoadGameObjects() {
        std::vector<LveModel::Vertex> vertices{
      {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
      {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}} };
        auto lveModel = std::make_shared<LveModel>(lveDevice, vertices);

        // https://www.color-hex.com/color-palette/5361
        std::vector<glm::vec3> colors{
            {1.f, .7f, .73f},
            {1.f, .87f, .73f},
            {1.f, 1.f, .73f},
            {.73f, 1.f, .8f},
            {.73, .88f, 1.f}  //
        };
        for (auto& color : colors) {
            color = glm::pow(color, glm::vec3{ 2.2f });
        }
        for (int i = 0; i < 40; i++) {
            auto triangle = LveGameObject::CreateGameObject();
            triangle.model = lveModel;
            triangle.transform2d.scale = glm::vec2(.5f) + i * 0.025f;
            triangle.transform2d.rotation = i * glm::pi<float>() * .025f;
            triangle.color = colors[i % colors.size()];
            gameObjects.push_back(std::move(triangle));
        }
    }

}	// namespace lve