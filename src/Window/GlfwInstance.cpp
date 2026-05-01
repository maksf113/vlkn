#include "window/GlfwInstance.hpp"

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

#include <stdexcept>

GlfwInstance::GlfwInstance()
{
	if (!glfwInit())
	{
		throw std::runtime_error("Failed to initialize GLFW");
	}
}

GlfwInstance::~GlfwInstance()
{
	glfwTerminate();
}