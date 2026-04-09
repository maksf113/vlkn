#include "Vulkan/Core/Fence.hpp"
#include "Vulkan/Core/Utility.hpp"

#include <stdexcept>

namespace vk::core
{
	Fence::Fence(const std::shared_ptr<Device>& device, bool signaled) : m_device(device)
	{
		VkFenceCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : static_cast<VkFenceCreateFlags>(0)
		};
		if (vkCreateFence(*m_device, &createInfo, nullptr, &m_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create fence");
		}
	}
	Fence::Fence(Fence&& other) noexcept :
		m_handle(other.m_handle), m_device(std::move(other.m_device))
	{
		other.m_handle = VK_NULL_HANDLE;
	}
	Fence& Fence::operator=(Fence&& other) noexcept
	{
		m_handle = other.m_handle;
		m_device = std::move(other.m_device);
		other.m_handle = VK_NULL_HANDLE;
		return *this;
	}
	Fence::~Fence()
	{
		if (m_handle != VK_NULL_HANDLE)
		{
			vkDestroyFence(*m_device, m_handle, nullptr);
		}
	}
	VkFence Fence::get() const
	{
		return m_handle;
	}
	Fence::operator VkFence() const
	{
		return m_handle;
	}
	void Fence::wait(uint64_t timeout) const
	{
		check(vkWaitForFences(*m_device, 1, &m_handle, VK_TRUE, timeout));
	}
	void Fence::reset() const
	{
		check(vkResetFences(*m_device, 1, &m_handle));
	}
}