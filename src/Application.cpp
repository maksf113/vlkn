#include "Application.hpp"
#include "Vulkan/Core/Instance.hpp"

#include <memory>

Application::Application(std::string_view name, uint16_t width, uint16_t height) :
	m_name(name)
{
	m_window = std::make_shared<Window>(width, height, name);
	m_vkContext = std::make_unique<vk::core::Context>(name, m_window);
}

Application::~Application()
{

}

void Application::run()
{
	while (!m_window->shouldClose())
	{
		m_window->pollEvents();
	}
}
