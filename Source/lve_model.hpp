#pragma once
#include "lve_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>

namespace lve {
	/// <summary>
	/// Take vertex data created by or read in a file from the CPU,
	/// then allocate mem and copy data over to GPU, so it can be rendered efficiently.
	/// </summary>
	class LveModel {
	public:
		struct Vertex {
			glm::vec3 position;
			glm::vec3 color;

			inline Vertex(glm::vec3 pos, glm::vec3 col) : position{ pos }, color{ col } {};
			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		};

		struct Data {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
		};

		LveModel(LveDevice &device, const LveModel::Data &data);
		~LveModel();
		LveModel(const LveModel&) = delete;	// manages memory buffer and vulkan objects, must delete
		LveModel& operator=(const LveModel&) = delete;

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);
	private:
		LveDevice& lveDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;	// memory management
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;	// memory management
		uint32_t indexCount;

		void CreateVertexBuffers(const std::vector<Vertex>& vertices);
		void CreateIndexBuffers(const std::vector<uint32_t>& indices);
	};
}	// namespace lve