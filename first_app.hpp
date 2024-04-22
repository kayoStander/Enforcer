#pragma once

#include "lve_swap_chain.hpp"
#include "lve_window.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"

#include <memory>
#include <vector>

namespace lve{
	class firstApp{
		public:
			static constexpr int WIDTH = 800;
			static constexpr int HEIGHT = 600;
			
			firstApp();
			~firstApp();

			firstApp(const firstApp&)=delete;
			firstApp &operator=(const firstApp&)=delete;

			void run();
		private:
			void createPipelineLayout();
			void createPipeline();
			void createCommandBuffers();
			void drawFrame();

			lveWindow lvewindow{WIDTH,HEIGHT,"Hello Vulkan!"};	
			lveDevice lvedevice{lvewindow};
			lveSwapChain lveswapchain{lvedevice,lvewindow.getExtent()};
			std::unique_ptr<lvePipeline> lvepipeline;		
			VkPipelineLayout pipelineLayout;
			std::vector<VkCommandBuffer> commandBuffers;
	};
}
