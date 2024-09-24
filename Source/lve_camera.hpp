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

		const glm::mat4& GetProjection() const { return projectionMatrix; }
	private:
		glm::mat4 projectionMatrix{ 1.0f };
	};
}	// namespace lve