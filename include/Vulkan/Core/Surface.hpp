#pragma once
#include "Vulkan/Core/Instance.hpp"
#include "Window/Window.hpp"

#include <Vulkan/vulkan.h>

#include <memory>

namespace vk
{
	class Surface
	{
	private:
			VkSurfaceKHR m_handle = VK_NULL_HANDLE;
			std::shared_ptr<Instance> m_instance;
			std::shared_ptr<Window> m_window;
	public:
		Surface(const std::shared_ptr<Instance>& instance, const std::shared_ptr<Window>& window);
		Surface(const Surface&) = delete;
		Surface& operator=(const Surface&) = delete;
		Surface(Surface&& other) noexcept;
		Surface& operator=(Surface&& other) noexcept;
		~Surface();
		VkSurfaceKHR get() const;
		operator VkSurfaceKHR() const;
	};
}