#pragma once

#include "lve_device.hpp"

#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace lve{
	struct pipelineConfigInfo{
		pipelineConfigInfo(const pipelineConfigInfo&) = delete;
		pipelineConfigInfo& operator=(const pipelineConfigInfo&) = delete;

		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};
	class lvePipeline{
		public:
			lvePipeline(
					lveDevice& device,
					const std::string& vertFilepath,
					const std::string& fragFilepath,
					const pipelineConfigInfo& configInfo);

			~lvePipeline();

			lvePipeline(const lvePipeline&) = delete;
			lvePipeline& operator=(const lveDevice&) = delete;

			void bind(VkCommandBuffer commandBuffer);
			static void defaultPipelineConfigInfo(pipelineConfigInfo& configInfo);

		private:
			static std::vector<char> readFile(const std::string& filepath);

			void createGraphicsPipeline(
					const std::string& vertFilepath,
					const std::string& fragFilepath,
					const pipelineConfigInfo& configInfo);

			void createShaderModule(
					const std::vector<char>&code,
					VkShaderModule* shaderModule);

			lveDevice& lvedevice;
			VkPipeline graphicsPipeline;
			VkShaderModule vertShaderModule;
			VkShaderModule fragShaderModule;

	};
}
