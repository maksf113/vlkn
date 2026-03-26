#include "Vulkan/Core/Surface.hpp"

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

namespace vk::core
{
	Surface::Surface(const std::shared_ptr<Instance>& instance, const std::shared_ptr<Window>& window) : 
		m_instance(instance), m_window(window)
	{
		if(glfwCreateWindowSurface(*m_instance.get(), *window, nullptr, &m_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create Vulkan surface");
		}
	}
	Surface::Surface(Surface&& other) noexcept :
		m_handle(other.m_handle), m_instance(std::move(other.m_instance))
	{
		other.m_handle = VK_NULL_HANDLE;
	}
	Surface& Surface::operator=(Surface&& other) noexcept
	{
		m_handle = other.m_handle;
		m_instance = std::move(other.m_instance);
		other.m_handle = VK_NULL_HANDLE;
		return *this;
	}
	Surface::~Surface()
	{
		if (m_handle != VK_NULL_HANDLE)
		{
			vkDestroySurfaceKHR(*m_instance, m_handle, nullptr);
		}
	}
	VkSurfaceKHR Surface::get() const
	{
		return m_handle;
	}
	Surface::operator VkSurfaceKHR() const
	{
		return m_handle;
	}
}