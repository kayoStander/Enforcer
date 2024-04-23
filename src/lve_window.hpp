#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace lve{
	class lveWindow{
		public:
			lveWindow(int w,int h, std::string name);
			~lveWindow();

			lveWindow(const lveWindow &)=delete;
			lveWindow operator=(const lveWindow &)=delete;

			bool shouldClose(){return glfwWindowShouldClose(window);}
			VkExtent2D getExtent() {return {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)};}
			bool wasWindowResized(){return framebufferResized;}
			void resetWindowResizedFlag(){framebufferResized = false;}

			void createWindowSurface(VkInstance instance,VkSurfaceKHR *surface);
		private:
			static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
			void initWindow();

			int width;
			int height;
			bool framebufferResized = false;

			std::string windowName;
			GLFWwindow *window;
	};
}
