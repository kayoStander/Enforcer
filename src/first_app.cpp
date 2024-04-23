#include "first_app.hpp"
#include "lve_game_object.hpp"
#include "lve_pipeline.hpp"
#include "lve_swap_chain.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <GLFW/glfw3.h>
#include <memory>
#include <stdexcept>
#include <array>
#include <vulkan/vulkan_core.h>

namespace lve{
	
	struct simplePushConstantData{
		glm::mat2 transform{1.f};
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	}; // max of 128 bytes

	firstApp::firstApp(){
		loadGameObjects();
		createPipelineLayout();
		createPipeline();
	}

	firstApp::~firstApp(){
		vkDestroyPipelineLayout(lvedevice.device(),pipelineLayout,nullptr);
	}

	void firstApp::run(){
		while (!lvewindow.shouldClose()){
			glfwPollEvents();
			
			if (auto commandBuffer = lverenderer.beginFrame()){

				// begin offscreen shadow pass 
				// render shadow casting objects
				// end offscreen shadow pass

				lverenderer.beginSwapChainRenderPass(commandBuffer);
				renderGameObjects(commandBuffer);
				lverenderer.endSwapChainRenderPass(commandBuffer);
				lverenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(lvedevice.device());
	};

	void firstApp::loadGameObjects(){
		// make an for loop to gen many vertices;
		std::vector<lveModel::Vertex> vertices{
			{{0.0f,-0.5f},{1.0f,0.0f,0.0f}},
			{{0.5f,0.5f},{0.0f,1.0f,0.0f}},
			{{-0.5f,0.5f},{0.0f,0.0f,1.0f}}
		};
		auto lvemodel = std::make_shared<lveModel>(lvedevice,vertices);

		auto triangle = lveGameObject::createGameObject();
		triangle.model = lvemodel;
		triangle.color = {.1f,.8f,.1f};
		triangle.transform2D.translation.x = .2f;
		triangle.transform2D.scale = {2.f , .5f};
		triangle.transform2D.rotation = .25f * glm::two_pi<float>(); // radians not degrees

		gameobjects.push_back(std::move(triangle));
	}

	void firstApp::createPipelineLayout(){
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

	void firstApp::createPipeline(){
		assert(pipelineLayout != nullptr && "cannot create pipeline before pipelinelayout!");

		pipelineConfigInfo pipelineConfig{};
		lvePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = lverenderer.getSwapChainRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvepipeline = std::make_unique<lvePipeline>(
			lvedevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig
			);
	}

		void firstApp::renderGameObjects(VkCommandBuffer commandBuffer){
		lvepipeline->bind(commandBuffer);

		for (auto &obj : gameobjects){
			obj.transform2D.rotation = glm::mod(obj.transform2D.rotation + 0.01f, glm::two_pi<float>());
			simplePushConstantData push{};
			push.offset = {0.0f,0.0f}; // offset: obj.transform2D.translation;
			push.color = obj.color;
			push.transform = obj.transform2D.mat2();

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

