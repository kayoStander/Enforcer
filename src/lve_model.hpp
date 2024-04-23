#pragma once

#include "lve_device.hpp"
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace lve{
	class lveModel{
		public:

			struct Vertex{
				glm::vec2 position;
				glm::vec3 color;

				static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
				static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
			};

			lveModel(lveDevice &device,const std::vector<Vertex> &vertices);
			~lveModel();

			lveModel(const lveModel&)=delete;
			lveModel &operator=(const lveModel&)=delete;
			
			void bind(VkCommandBuffer commandBuffer);
			void draw(VkCommandBuffer commandBuffer);
		private:
			lveDevice &lvedevice;
			VkBuffer vertexBuffer;
			VkDeviceMemory vertexBufferMemory;
			uint32_t vertexCount;

			void createVertexBuffers(const std::vector<Vertex> &vertices);
	};
}
