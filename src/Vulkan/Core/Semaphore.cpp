#include "Vulkan/Core/Semaphore.hpp"

#include <stdexcept>

namespace vk::core
{
	Semaphore::Semaphore(const std::shared_ptr<Device>& device) : m_device(device)
	{
		VkSemaphoreCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
		};
		if (vkCreateSemaphore(*m_device, &createInfo, nullptr, &m_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create semaphore");
		}
	}

	Semaphore::Semaphore(Semaphore&& other) noexcept :
		m_handle(other.m_handle), m_device(std::move(other.m_device))
	{
		other.m_handle = VK_NULL_HANDLE;
	}	

	Semaphore& Semaphore::operator=(Semaphore&& other) noexcept
	{
		m_handle = other.m_handle;
		m_device = std::move(other.m_device);
		other.m_handle = VK_NULL_HANDLE;
		return *this;
	}	

	Semaphore::~Semaphore()
	{
		if (m_handle != VK_NULL_HANDLE)
		{
			vkDestroySemaphore(*m_device, m_handle, nullptr);
		}
	}	

	VkSemaphore Semaphore::get() const
	{
		return m_handle;
	}	

	Semaphore::operator VkSemaphore() const
	{
		return m_handle;
	}
}