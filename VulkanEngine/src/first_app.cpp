#include "first_app.hpp"
#include "Systems/simple_render_system.hpp"
#include "Systems/point_light_system.hpp"
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
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            LveDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }
        SimpleRenderSystem renderSystem(lveDevice, lveRenderer.GetSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
        PointLightSystem pointLightSystem(lveDevice, lveRenderer.GetSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
        LveCamera camera{};

        auto viewerObject = LveGameObject::CreateGameObject();
        viewerObject.transform.translation.z = -2.5f;
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
            camera.SetPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);

            if (auto commandBuffer = lveRenderer.BeginFrame()) {
                int frameIndex = lveRenderer.GetFrameIndex();
                FrameInfo frameInfo{ 
                    frameIndex, 
                    frameTime,
                    commandBuffer,
                    camera, 
                    globalDescriptorSets[frameIndex],
                    gameObjects
                };

                // update
                GlobalUbo ubo{};
                ubo.projection = camera.GetProjection();
                ubo.view = camera.GetView();
                ubo.inverseView = camera.GetInverseView();
                pointLightSystem.Update(frameInfo, ubo);
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                lveRenderer.BeginSwapChainRenderPass(commandBuffer);

                // order matters
                renderSystem.RenderGameObjects(frameInfo);
                pointLightSystem.Render(frameInfo);
                lveRenderer.EndSwapChainRenderPass(commandBuffer);
                lveRenderer.EndFrame();
            }
		}
        vkDeviceWaitIdle(lveDevice.device());
	}

    void FirstApp::LoadGameObjects() {
        std::shared_ptr<LveModel> lveModel =
            LveModel::CreateModelFromFile(lveDevice, "models/flat_vase.obj");
        auto flatVase = LveGameObject::CreateGameObject();
        flatVase.model = lveModel;
        flatVase.transform.translation = { -.5f, .5f, 0.f };
        flatVase.transform.scale = 3.0f;
        gameObjects.emplace(flatVase.GetId(), std::move(flatVase));

        lveModel = LveModel::CreateModelFromFile(lveDevice, "models/smooth_vase.obj");
        auto smoothVase = LveGameObject::CreateGameObject();
        smoothVase.model = lveModel;
        smoothVase.transform.translation = { .5f, .5f, 0.f };
        smoothVase.transform.scale = 3.0f;
        gameObjects.emplace(smoothVase.GetId(), std::move(smoothVase));

        lveModel = LveModel::CreateModelFromFile(lveDevice, "models/quad.obj");
        auto floor = LveGameObject::CreateGameObject();
        floor.model = lveModel;
        floor.transform.translation = { 0.f, .5f, 0.f };
        floor.transform.scale = 3.0f;
        gameObjects.emplace(floor.GetId(), std::move(floor));
        {
            auto pointLight = LveGameObject::MakePointLight(0.2f);

            std::vector<glm::vec3> lightColors{
                 {1.f, .1f, .1f},
                 {.1f, .1f, 1.f},
                 {.1f, 1.f, .1f},
                 {1.f, 1.f, .1f},
                 {.1f, 1.f, 1.f},
                 {1.f, 1.f, 1.f}  //
            };

            for (int i = 0;  i < lightColors.size(); i++) {
                auto pointLight = LveGameObject::MakePointLight(0.2f);
                pointLight.color = lightColors[i];
                auto rotateLight = glm::rotate(
                    glm::mat4(1.0f),
                    (i * glm::two_pi<float>()) / lightColors.size(),
                    { 0.0f, -1.0f, 0.0f }
                );
                pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
                gameObjects.emplace(pointLight.GetId(), std::move(pointLight));
            }
        }
    }
}	// namespace lve