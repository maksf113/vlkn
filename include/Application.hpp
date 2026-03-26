#pragma once
#include "Vulkan/Core/Context.hpp"
#include "Window/GlfwInstance.hpp"
#include "Window/Window.hpp"

#include <string>
#include <memory>

class Application
{
private:
	std::string m_name;
	GlfwInstance m_glfwInstance;
	std::shared_ptr<Window> m_window;
	std::unique_ptr<vk::core::Context> m_vkContext;
	
public:
	Application(std::string_view name, uint16_t width, uint16_t height);
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
	~Application();

	void run();
};