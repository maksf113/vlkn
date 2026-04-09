#include "Vulkan/Core/CommandPool.hpp"

#include <stdexcept>

namespace vk::core
{
	CommandPool::CommandPool(const std::shared_ptr<Device>& device) : m_device(device)
	{
		VkCommandPoolCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = m_device->getPhysicalDevice()->getQueueFamilyIndices().graphicsFamily.value()
		};
		if (vkCreateCommandPool(*m_device, &createInfo, nullptr, &m_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create command pool");
		}
	}

	CommandPool::CommandPool(CommandPool&& other) noexcept :
		m_handle(other.m_handle), m_device(std::move(other.m_device))
	{
		other.m_handle = VK_NULL_HANDLE;
	}

	CommandPool& CommandPool::operator=(CommandPool&& other) noexcept
	{
		m_handle = other.m_handle;
		m_device = std::move(other.m_device);
		other.m_handle = VK_NULL_HANDLE;
		return *this;
	}

	CommandPool::~CommandPool()
	{
		if (m_handle != VK_NULL_HANDLE)
		{
			vkDestroyCommandPool(*m_device, m_handle, nullptr);
		}
	}

	VkCommandPool CommandPool::get() const
	{
		return m_handle;
	}

	CommandPool::operator VkCommandPool() const
	{
		return m_handle;
	}
}