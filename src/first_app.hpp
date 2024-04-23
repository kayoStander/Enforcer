#pragma once

#include "lve_renderer.hpp"
#include "lve_window.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"

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
			void loadGameObjects();
			void createPipelineLayout();
			void createPipeline();
			void renderGameObjects(VkCommandBuffer commandBuffer);

			lveWindow lvewindow{WIDTH,HEIGHT,"Hello Vulkan!"};	
			lveDevice lvedevice{lvewindow};
			lveRenderer lverenderer{lvewindow,lvedevice};
			std::unique_ptr<lvePipeline> lvepipeline;		
			VkPipelineLayout pipelineLayout;
			std::vector<lveGameObject> gameobjects;
	};
}
