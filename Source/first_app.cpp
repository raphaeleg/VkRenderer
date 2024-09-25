#include "first_app.hpp"
#include "simple_render_system.hpp"
#include "keyboard_movement_controller.hpp"
#include "lve_camera.hpp"
#include "lve_buffer.hpp"

#include <stdexcept>
#include <array>
#include <chrono>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace lve{
    struct GlobalUbo {
        alignas(16) glm::mat4 projectionView{ 1.0f };
        alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3{1.0f, -3.0f, -1.0f});
    };

    static constexpr float MAX_FRAME_TIME = 60.f;

    FirstApp::FirstApp() { 
        globalPool = LveDescriptorPool::Builder(lveDevice)
            .setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
        LoadGameObjects(); 
    }

	void FirstApp::run() {
        std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<LveBuffer>(
                lveDevice,
                sizeof(GlobalUbo),
                1, // double buffering
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->map();
        }

        auto globalSetLayout = LveDescriptorSetLayout::Builder(lveDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
            .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            LveDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }
        SimpleRenderSystem renderSystem(lveDevice, lveRenderer.GetSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
        LveCamera camera{};

        auto viewerObject = LveGameObject::CreateGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

		while (!lveWindow.shouldClose()) {
			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            frameTime = glm::min(frameTime, MAX_FRAME_TIME);

            cameraController.MoveInPlaneXZ(lveWindow.GetGLFWwindow(), frameTime, viewerObject);
            camera.SetViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = lveRenderer.GetAspectRatio();
            camera.SetPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);

            if (auto commandBuffer = lveRenderer.BeginFrame()) {
                int frameIndex = lveRenderer.GetFrameIndex();
                FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex]};

                // update
                GlobalUbo ubo{};
                ubo.projectionView = camera.GetProjection() * camera.GetView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                lveRenderer.BeginSwapChainRenderPass(commandBuffer);
                renderSystem.RenderGameObjects(frameInfo, gameObjects);
                lveRenderer.EndSwapChainRenderPass(commandBuffer);
                lveRenderer.EndFrame();
            }
		}
        vkDeviceWaitIdle(lveDevice.device());
	}

    void FirstApp::LoadGameObjects() {
        std::shared_ptr<LveModel> lveModel =
            LveModel::CreateModelFromFile(lveDevice, "models/flat_vase.obj");
        auto gameObj = LveGameObject::CreateGameObject();
        gameObj.model = lveModel;
        gameObj.transform.translation = { 0.0f, 0.5f, 2.5f };
        gameObj.transform.scale = 2.0f;
        gameObjects.push_back(std::move(gameObj));
    }
}	// namespace lve