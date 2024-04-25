#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "lve_camera.hpp"
#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <GLFW/glfw3.h>
#include <memory>
#include <stdexcept>
#include <array>
#include <chrono>

#include <iostream>

namespace lve{	

	firstApp::firstApp(){
		loadGameObjects();
	}

	firstApp::~firstApp(){
		
	}

	void firstApp::run(){
		simpleRenderSystem simplerendersystem{lvedevice,lverenderer.getSwapChainRenderPass()};
		lveCamera camera{};
		camera.setViewTarget(glm::vec3{-1.f,-2.f,2.f},glm::vec3{0.f,0.f,2.5f}); // camera pos

		auto viewerObject = lveGameObject::createGameObject();
		keyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!lvewindow.shouldClose()){
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = 
				std::chrono::duration<float,std::chrono::seconds::period>(newTime-currentTime).count(); // deltaTime
			currentTime = newTime;

			cameraController.moveInPlaneXYZ(lvewindow.getGLFWwindow(),frameTime,viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation,viewerObject.transform.rotation);

			float aspect = lverenderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.f),aspect,.1f,10.f); // cube not be squashed, also increase far plane distance to see more away.

			if (auto commandBuffer = lverenderer.beginFrame()){

				// begin offscreen shadow pass 
				// render shadow casting objects
				// end offscreen shadow pass

				lverenderer.beginSwapChainRenderPass(commandBuffer);
				simplerendersystem.renderGameObjects(commandBuffer,gameobjects,camera);
				lverenderer.endSwapChainRenderPass(commandBuffer);
				lverenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(lvedevice.device());
	};

	std::unique_ptr<lveModel> createCubeModel(lveDevice& device, glm::vec3 offset) {
		lveModel::Builder modelBuilder{};

		for (auto& v : modelBuilder.vertices) {
			v.position += offset;
		}
		modelBuilder.indices = 
		{0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
        12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};

		return std::make_unique<lveModel>(device, modelBuilder);
	}


	void firstApp::loadGameObjects(){
		std::shared_ptr<lveModel> lveModel = 
			lveModel::createModelFromFile(lvedevice,"models/PlatinaSword.obj");

		auto cube = lveGameObject::createGameObject();
		cube.model = lveModel;
		cube.transform.translation = {.0f,.0f,2.5f}; // X -Y Z
		cube.transform.scale = {.5f,.5f,.5f};
		gameobjects.push_back(std::move(cube));
	}
}

