#include "Vulkan/Core/Context.hpp"

#include <iostream>
#include <vector>
#include <stdexcept>
#include <memory>
#include <utility>

namespace vk
{
	Context::Context(std::string_view name, std::shared_ptr<Window>& window) : 
		m_name(name), m_window(window)
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		m_instanceConfig.appName = name;
		m_instanceConfig.layers = {
				"VK_LAYER_KHRONOS_validation"
		};
		m_instanceConfig.extensions = extensions;
#ifdef DEBUG_MODE
		m_instanceConfig.enableValidation = true;
#else
		m_instanceConfig.enableValidation = false;
#endif

		m_vkInstance = std::make_shared<Instance>(m_instanceConfig);
		m_debugMessenger = std::make_unique<DebugMessenger>(m_vkInstance);
		m_surface = std::make_shared<Surface>(m_vkInstance, m_window);
		m_physicalDevice = std::make_shared<PhysicalDevice>(m_vkInstance, m_surface);
		m_device = std::make_shared<Device>(m_physicalDevice);
	}

	const std::shared_ptr<Device>& Context::getDevice() const
	{
		return m_device;
	}

	const std::shared_ptr<Window>& Context::getWindow() const
	{
		return m_window;
	}	

	const std::shared_ptr<Surface>& Context::getSurface() const
	{
		return m_surface;
	}
}