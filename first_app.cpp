#include "first_app.hpp"
#include "lve_pipeline.hpp"
#include "lve_swap_chain.hpp"
#include <memory>
#include <stdexcept>
#include <array>
#include <vulkan/vulkan_core.h>

namespace lve{

	firstApp::firstApp(){
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
	}

	firstApp::~firstApp(){
		vkDestroyPipelineLayout(lvedevice.device(),pipelineLayout,nullptr);

	}

	void firstApp::run(){
		while (!lvewindow.shouldClose()){
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(lvedevice.device());
	};

	void firstApp::createPipelineLayout(){
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(lvedevice.device(),&pipelineLayoutInfo,nullptr,&pipelineLayout)
				!= VK_SUCCESS){
			throw std::runtime_error("failed to create pipeline layout");
		}
	}

	void firstApp::createPipeline(){
		auto pipelineConfig = lvePipeline::defaultPipelineConfigInfo(
				lveswapchain.width(),lveswapchain.height());	
		pipelineConfig.renderPass = lveswapchain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvepipeline = std::make_unique<lvePipeline>(
				lvedevice,
				"shaders/simple_shader.vert.spv",
				"shaders/simple_shader.frag.spv",
				pipelineConfig);
	}

	void firstApp::createCommandBuffers(){
		commandBuffers.resize(lveswapchain.imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = lvedevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
		if (vkAllocateCommandBuffers(lvedevice.device(),&allocInfo,commandBuffers.data())
				!= VK_SUCCESS){
			throw std::runtime_error("failed to allocate command buffers");
		}

		for (int i=0;i<commandBuffers.size();i++){
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffers[i],&beginInfo) != VK_SUCCESS){
				throw std::runtime_error("failed to begin recording command buffer");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = lveswapchain.getRenderPass();
			renderPassInfo.framebuffer = lveswapchain.getFrameBuffer(i);

			renderPassInfo.renderArea.offset = {0,0};
			renderPassInfo.renderArea.extent = lveswapchain.getSwapChainExtent();
			
			std::array<VkClearValue,2> clearValues{};
			clearValues[0].color = {0.1f,0.1f,0.1f,1.0f};
			clearValues[1].depthStencil = {1.0f , 0};
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[i],&renderPassInfo,VK_SUBPASS_CONTENTS_INLINE);

			lvepipeline->bind(commandBuffers[i]);
			vkCmdDraw(commandBuffers[i],3,1,0,0);

			vkCmdEndRenderPass(commandBuffers[i]);
			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS){
				throw std::runtime_error("failed to record command buffer");
			}
		}
	}
	void firstApp::drawFrame(){
		uint32_t imageIndex;
		auto result = lveswapchain.acquireNextImage(&imageIndex);
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
			throw std::runtime_error("failed to acquire next swap chain image");
		}
		result = lveswapchain.submitCommandBuffers(&commandBuffers[imageIndex],&imageIndex);
		if (result != VK_SUCCESS){
			throw std::runtime_error("failed to present swap chain image");
		}
	}
}

