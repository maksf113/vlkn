#include "Application.hpp"

#include "vulkan/core/Instance.hpp"
#include "vulkan/Utility.hpp"

#include <Vulkan/vulkan.h>
#include "vulkan/vk_enum_string_helper.h"

#include <memory>

Application::Application(std::string_view name, uint16_t width, uint16_t height) :
	m_name(name)
{
	m_window = std::make_shared<Window>(width, height, name);
	m_vkContext = std::make_shared<vk::Context>(name, m_window);
	m_renderer = std::make_shared<vk::Renderer>(m_vkContext);
	m_inputManager.setCallbacks(m_vkContext->getWindow()->get());
	m_inputManager.addReceiver(m_renderer);
}

Application::~Application()
{

}


void Application::run()
{
	while (!m_window->shouldClose())
	{
		m_window->pollEvents();
		m_renderer->processInput(m_inputManager);
		m_renderer->drawFrame();
		m_inputManager.endFrame();
	}
	vkDeviceWaitIdle(m_vkContext->getDevice()->get());
}
