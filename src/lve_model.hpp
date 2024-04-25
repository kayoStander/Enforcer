#pragma once

#include "lve_device.hpp"

#include <memory>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace lve{
	class lveModel{
		public:

			struct Vertex{
				glm::vec3 position{};
				glm::vec3 color{};
				glm::vec3 normal{};
				glm::vec2 uv{};

				static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
				static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
			};

			struct Builder{
				std::vector<Vertex> vertices{};
				std::vector<uint32_t> indices{};

				void loadModel(const std::string &filePath);
			};

			lveModel(lveDevice &device,const lveModel::Builder &builder);
			~lveModel();

			lveModel(const lveModel&)=delete;
			lveModel &operator=(const lveModel&)=delete;

			static std::unique_ptr<lveModel> createModelFromFile(
					lveDevice &device
					,const std::string &filePath);
			
			void bind(VkCommandBuffer commandBuffer);
			void draw(VkCommandBuffer commandBuffer);
		private:
			void createVertexBuffers(const std::vector<Vertex> &vertices);
			void createIndexBuffers(const std::vector<uint32_t> &indices);

			lveDevice &lvedevice;

			VkBuffer vertexBuffer;
			VkDeviceMemory vertexBufferMemory;
			uint32_t vertexCount;

			bool hasIndexBuffer = false;
			VkBuffer indexBuffer;
			VkDeviceMemory indexBufferMemory;
			uint32_t indexCount;
	};
}
