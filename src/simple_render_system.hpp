#pragma once

#include "lve_camera.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"

#include <memory>
#include <vector>

namespace lve{
	class simpleRenderSystem{
		public:
			
			simpleRenderSystem(lveDevice& device,VkRenderPass renderPass);
			~simpleRenderSystem();

			simpleRenderSystem(const simpleRenderSystem&)=delete;
			simpleRenderSystem &operator=(const simpleRenderSystem&)=delete;

			void renderGameObjects(
					VkCommandBuffer commandBuffer,
					std::vector<lveGameObject>& gameObjects,
					lveCamera &camera);

		private:
			void createPipelineLayout();
			void createPipeline(VkRenderPass renderPass);

			lveDevice& lvedevice;
			std::unique_ptr<lvePipeline> lvepipeline;		
			VkPipelineLayout pipelineLayout;
	};
}
