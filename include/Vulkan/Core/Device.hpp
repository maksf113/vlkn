#pragma once
#include "Vulkan/Core/PhysicalDevice.hpp"

#include <Vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace vk::core
{
	class Device
	{
	private:
		VkDevice m_handle = VK_NULL_HANDLE;
		VkQueue m_graphicsQueue = VK_NULL_HANDLE;
		VkQueue m_presentQueue = VK_NULL_HANDLE;
		std::shared_ptr<PhysicalDevice> m_physicalDevice;

	public:
		Device(const std::shared_ptr<PhysicalDevice>& physicalDevice);
		Device(const Device&) = delete;
		Device& operator=(const Device&) = delete;
		Device(Device&& other) noexcept;
		Device& operator=(Device&& other) noexcept;
		~Device();
		VkDevice get() const;
		operator VkDevice() const;
		VkQueue getGraphicsQueue() const;
		VkQueue getPresentQueue() const;
		const std::shared_ptr<PhysicalDevice>& getPhysicalDevice() const;
	};
}