#include "lve_renderer.hpp"

#include <stdexcept>
#include <array>
#include <stdexcept>

namespace lve{
	
	lveRenderer::lveRenderer(lveWindow& window,lveDevice& device)
		:lvewindow{window},lvedevice{device}{
		recreateSwapChain();
		createCommandBuffers();
	}

	lveRenderer::~lveRenderer(){
		freeCommandBuffers();
	}

	void lveRenderer::recreateSwapChain(){
		auto extent = lvewindow.getExtent();

		while (extent.width == 0 || extent.height == 0){
			extent = lvewindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(lvedevice.device());

		if (lveswapchain == nullptr){
			vkDeviceWaitIdle(lvedevice.device());
			lveswapchain = std::make_unique<lveSwapChain>(lvedevice, extent);
		}else{
			lveswapchain = std::make_unique<lveSwapChain>(lvedevice, extent, std::move(lveswapchain));
			if (lveswapchain->imageCount() != commandbuffers.size()){
				freeCommandBuffers();
				createCommandBuffers();
			}
		}
		// ,,,
	}

	void lveRenderer::freeCommandBuffers(){
		vkFreeCommandBuffers(lvedevice.device(),
				lvedevice.getCommandPool(),
				static_cast<uint32_t>(commandbuffers.size()),
				commandbuffers.data());
		commandbuffers.clear();
	}

	void lveRenderer::createCommandBuffers(){
		commandbuffers.resize(lveswapchain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = lvedevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandbuffers.size());
		if (vkAllocateCommandBuffers(lvedevice.device(),&allocInfo,commandbuffers.data())
				!= VK_SUCCESS){
			throw std::runtime_error("failed to allocate command buffers");
		}
	}

	
	VkCommandBuffer lveRenderer::beginFrame(){
		assert(!isFrameStarted && "cannot call beginframe while alredy in progress");

		auto result = lveswapchain->acquireNextImage(&currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR){
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
			throw std::runtime_error("failed to acquire swap chain image");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS){
			throw std::runtime_error("failed to begin recording command buffer");
		}

		return commandBuffer;
	}	
	void lveRenderer::endFrame(){
		assert(isFrameStarted && "cant call endframe while frame is not in progress");
		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS){
			throw std::runtime_error("failed to record command buffer");
		}

		auto result = lveswapchain->submitCommandBuffers(&commandBuffer,&currentImageIndex);
		if (result == 
				VK_ERROR_OUT_OF_DATE_KHR 
				|| result == VK_SUBOPTIMAL_KHR 
				|| lvewindow.wasWindowResized()){
			lvewindow.resetWindowResizedFlag();
			recreateSwapChain();
		}else if(result != VK_SUCCESS){
			throw std::runtime_error("failed to present swap chain image");
		}

		isFrameStarted = false;
	}
	void lveRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer){
		assert(isFrameStarted && "cant call beginswapchainrenderpass if frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && 
				"cant being renderpass on command buffer from an different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = lveswapchain->getRenderPass();
		renderPassInfo.framebuffer = lveswapchain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = {0,0};
		renderPassInfo.renderArea.extent = lveswapchain->getSwapChainExtent();

		std::array<VkClearValue,2> clearValues{};
		clearValues[0].color = {0.01f,0.01f,0.01f,1.0f};
		clearValues[1].depthStencil = {1.0f,0};
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer,&renderPassInfo,VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(lveswapchain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(lveswapchain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{{0,0},lveswapchain->getSwapChainExtent()};
		vkCmdSetViewport(commandBuffer,0,1,&viewport);
		vkCmdSetScissor(commandBuffer,0,1,&scissor);
	}
	void lveRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer){
		assert(isFrameStarted && "cant call endswapchainrenderpass if frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && 
				"cant end renderpass on command buffer from an different frame");
		vkCmdEndRenderPass(commandBuffer);

	}
}

