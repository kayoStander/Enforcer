#pragma once

#include "lve_device.hpp"

#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace lve{
	struct pipelineConfigInfo{
		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};
	class lvePipeline{
		public:
			lvePipeline(
					lveDevice &device,
					const std::string& vertFilepath,
					const std::string& fragFilepath,
					const pipelineConfigInfo& configInfo);

			~lvePipeline();

			lvePipeline(const lvePipeline&) = delete;
			void operator=(const lveDevice&) = delete;

			void bind(VkCommandBuffer commandBuffer);
			static pipelineConfigInfo defaultPipelineConfigInfo(
					uint32_t width,
					uint32_t height);

		private:
			static std::vector<char> readFile(const std::string& filepath);

			void createGraphicsPipeline(
					const std::string& vertFilepath,
					const std::string& fragFilepath,
					const pipelineConfigInfo configInfo);

			void createShaderModule(
					const std::vector<char>&code,
					VkShaderModule* shaderModule);

			lveDevice& lvedevice;
			VkPipeline graphicsPipeline;
			VkShaderModule vertShaderModule;
			VkShaderModule fragShaderModule;

	};
}
