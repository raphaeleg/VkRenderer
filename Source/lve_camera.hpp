#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace lve {
	struct OrthographicVolume {
		float left = 0;
		float right = 0;
		float top = 0;
		float bottom = 0;
		float near = 0;
		float far = 0;

		OrthographicVolume(float l, float r, float t, float b, float n, float f)
			: left(l), right(r), top(t), bottom(b), near(n), far(f) {}
	};
	class LveCamera {
	public:
		void SetOrthographicProjection(OrthographicVolume planes);
		void SetPerspectiveProjection(float fovy, float aspect, float near, float far);

		void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{ 0.0f, -1.0f, 0.0f });
		void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{ 0.0f, -1.0f, 0.0f });
		void SetViewYXZ(glm::vec3 position, glm::vec3 rotation);
		const glm::mat4& GetProjection() const { return projectionMatrix; }
		const glm::mat4& GetView() const { return viewMatrix; }
		const glm::mat4& GetInverseView() const { return inverseViewMatrix; }
		const glm::vec3 GetPosition() const { return glm::vec3(inverseViewMatrix[3]); }
	private:
		glm::mat4 projectionMatrix{ 1.0f };
		glm::mat4 viewMatrix{ 1.0f };
		glm::mat4 inverseViewMatrix{ 1.0f };
	};
}	// namespace lve