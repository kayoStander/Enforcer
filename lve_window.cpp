#include "lve_window.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>

namespace lve{
	lveWindow::lveWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name} {
		initWindow();
	}

	lveWindow::~lveWindow(){
		glfwDestroyWindow(window);
		glfwTerminate();
	}	

	void lveWindow::initWindow(){
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);

		window = glfwCreateWindow(width,height,windowName.c_str(),nullptr,nullptr);
	}

	void lveWindow::createWindowSurface(VkInstance instance,VkSurfaceKHR *surface){
		if (glfwCreateWindowSurface(instance,window,nullptr,surface) != VK_SUCCESS){
			throw std::runtime_error("failed to create window surface");
		}
	}
}
