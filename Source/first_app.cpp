#include "first_app.hpp"
#include "simple_render_system.hpp"
#include "rainbow_system.hpp"
#include <stdexcept>
#include <array>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace lve{

	void FirstApp::run() {
        SimpleRenderSystem renderSystem(lveDevice, lveRenderer.GetSwapChainRenderPass());
        RainbowSystem rainbowSystem{ 0.5f };

		while (!lveWindow.shouldClose()) {
			glfwPollEvents();

            if (auto commandBuffer = lveRenderer.BeginFrame()) {
                // begin off-screen shadow pass
                // render shadow casting objects
                // end off-screen shadow pass

                lveRenderer.BeginSwapChainRenderPass(commandBuffer);
                renderSystem.RenderGameObjects(commandBuffer, gameObjects);
                rainbowSystem.update(1.f / 60, gameObjects);
                lveRenderer.EndSwapChainRenderPass(commandBuffer);
                lveRenderer.EndFrame();
            }
		}
        vkDeviceWaitIdle(lveDevice.device());
	}

    void FirstApp::LoadGameObjects() {
        std::vector<LveModel::Vertex> vertices{
          {{  0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f }},
          {{  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }},
          {{ -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }}
        };
        auto lveModel = std::make_shared<LveModel>(lveDevice, vertices);

        auto triangle = LveGameObject::CreateGameObject();
        triangle.model = lveModel;
        triangle.color = { 0.1f, 0.8f, 0.1f };
        triangle.transform2d.translation.x = 0.2f;
        triangle.transform2d.scale = {2.0f, 0.5f };
        triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
    }
}	// namespace lve