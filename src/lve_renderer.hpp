#pragma once

#include "lve_swap_chain.hpp"
#include "lve_window.hpp"
#include "lve_device.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace lve{
	class lveRenderer{
		public:
			lveRenderer(lveWindow& window, lveDevice& device);
			~lveRenderer();

			lveRenderer(const lveRenderer&)=delete;
			lveRenderer &operator=(const lveRenderer&)=delete;

			VkRenderPass getSwapChainRenderPass() const {return lveswapchain->getRenderPass();}
			float getAspectRatio() const {return lveswapchain->extentAspectRatio();}
			bool isFrameInProgress() const {return isFrameStarted;}

			VkCommandBuffer getCurrentCommandBuffer() const {
				assert(isFrameStarted && "cannot get commandbuffer when frame is not on progress");
				return commandbuffers[currentFrameIndex];}

			int getFrameIndex() const {
				assert(isFrameStarted && "cannot call while frame is not on progress");

				return currentFrameIndex;
			}

			VkCommandBuffer beginFrame();
			void endFrame();
			void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
			void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

		private:
			void createCommandBuffers();
			void freeCommandBuffers();
			void recreateSwapChain();

			lveWindow& lvewindow;	
			lveDevice& lvedevice;
			std::unique_ptr<lveSwapChain> lveswapchain;
			std::vector<VkCommandBuffer> commandbuffers;

			uint32_t currentImageIndex;
			int currentFrameIndex;
			bool isFrameStarted = false;
			//uint32_t deltaTime; do in future, lastframe-nowframe
	};
}
