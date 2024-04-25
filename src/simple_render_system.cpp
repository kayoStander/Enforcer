#include "simple_render_system.hpp"
#include "lve_game_object.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <GLFW/glfw3.h>
#include <memory>
#include <stdexcept>
#include <array>

namespace lve{
	
	struct simplePushConstantData{
		glm::mat4 transform{1.f};
		alignas(16) glm::vec3 color;
	}; // max of 128 bytes

	simpleRenderSystem::simpleRenderSystem(lveDevice& device,VkRenderPass renderPass):
		lvedevice{device}{
		createPipelineLayout();
		createPipeline(renderPass);
	}

	simpleRenderSystem::~simpleRenderSystem(){
		vkDestroyPipelineLayout(lvedevice.device(),pipelineLayout,nullptr);
	}

	void simpleRenderSystem::createPipelineLayout(){
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = 
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(simplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(lvedevice.device(),&pipelineLayoutInfo,nullptr,&pipelineLayout)
				!= VK_SUCCESS){
			throw std::runtime_error("failed to create pipeline layout");
		}
	}

	void simpleRenderSystem::createPipeline(VkRenderPass renderPass){
		assert(pipelineLayout != nullptr && "cannot create pipeline before pipelinelayout!");

		pipelineConfigInfo pipelineConfig{};
		lvePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvepipeline = std::make_unique<lvePipeline>(
			lvedevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig
			);
	}

		void simpleRenderSystem::renderGameObjects(
				VkCommandBuffer commandBuffer
				,std::vector<lveGameObject> &gameObjects,
				lveCamera &camera){
		lvepipeline->bind(commandBuffer);

		auto projectionView = camera.getProjection() * camera.getView();

		for (auto &obj : gameObjects){
			simplePushConstantData push{};
			push.color = obj.color;
			push.transform = projectionView * obj.transform.mat4();

			vkCmdPushConstants(
				commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(simplePushConstantData),
				&push
				);
			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);
		}
	}
}

