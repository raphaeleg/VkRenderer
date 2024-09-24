#include "first_app.hpp"
#include "simple_render_system.hpp"
#include "keyboard_movement_controller.hpp"
#include "lve_camera.hpp"
#include <stdexcept>
#include <array>
#include <chrono>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace lve{
    static constexpr float MAX_FRAME_TIME = 60.f;
	void FirstApp::run() {
        SimpleRenderSystem renderSystem(lveDevice, lveRenderer.GetSwapChainRenderPass());
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
            //camera.SetOrthographicProjection({ -aspect, aspect, -1, 1, -1, 1 });
            camera.SetPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);

            if (auto commandBuffer = lveRenderer.BeginFrame()) {
                // begin off-screen shadow pass
                // render shadow casting objects
                // end off-screen shadow pass

                lveRenderer.BeginSwapChainRenderPass(commandBuffer);
                renderSystem.RenderGameObjects(commandBuffer, gameObjects, camera);
                lveRenderer.EndSwapChainRenderPass(commandBuffer);
                lveRenderer.EndFrame();
            }
		}
        vkDeviceWaitIdle(lveDevice.device());
	}

    void FirstApp::LoadGameObjects() {
        std::shared_ptr<LveModel> lveModel =
            LveModel::CreateModelFromFile(lveDevice, "models/smooth_vase.obj");
        auto gameObj = LveGameObject::CreateGameObject();
        gameObj.model = lveModel;
        gameObj.transform.translation = { .0f, .0f, 2.5f };
        gameObj.transform.scale = glm::vec3(3.f);
        gameObjects.push_back(std::move(gameObj));
    }
}	// namespace lve