#include "Application.hpp"
#include "Vulkan/Core/Instance.hpp"
#include "Vulkan/Utility.hpp"

#include <Vulkan/vulkan.h>
#include "Vulkan/vk_enum_string_helper.h"

#include <memory>

Application::Application(std::string_view name, uint16_t width, uint16_t height) :
	m_name(name)
{
	m_window = std::make_shared<Window>(width, height, name);
	m_vkContext = std::make_shared<vk::Context>(name, m_window);
	m_renderer = std::make_unique<vk::Renderer>(m_vkContext);
}

Application::~Application()
{

}


void Application::run()
{
	while (!m_window->shouldClose())
	{
		m_window->pollEvents();
		m_renderer->drawFrame();
	}
}
