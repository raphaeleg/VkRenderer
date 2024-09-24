#include "lve_camera.hpp"
#include <cassert>
#include <limits>

namespace lve{
	void LveCamera::SetOrthographicProjection(OrthographicVolume viewVolume) {
		const float rl = viewVolume.right - viewVolume.left;
		const float bt = viewVolume.bottom - viewVolume.top;
		const float fn = viewVolume.far - viewVolume.near;

		projectionMatrix = glm::mat4{ 1.0f };
		projectionMatrix[0][0] = 2.f / rl;	// vulkan canonical view = {2,2,1}
		projectionMatrix[1][1] = 2.f / bt;
		projectionMatrix[2][2] = 1.f / fn;
		projectionMatrix[3][0] = -(viewVolume.right + viewVolume.left) / rl;
		projectionMatrix[3][1] = -(viewVolume.bottom + viewVolume.top) / bt;
		projectionMatrix[3][2] = -viewVolume.near / fn;
	}

	void LveCamera::SetPerspectiveProjection(float fovy, float aspect, float near, float far) {
		assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
		const float tanHalfFovy = tan(fovy / 2.f);

		projectionMatrix = glm::mat4{ 0.0f };
		projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
		projectionMatrix[1][1] = 1.f / (tanHalfFovy);
		projectionMatrix[2][2] = far / (far - near);
		projectionMatrix[2][3] = 1.f;
		projectionMatrix[3][2] = -(far * near) / (far - near);
	}
}	// namespace lve