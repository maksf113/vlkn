#include "Vulkan/Core/Device.hpp"
#include "Vulkan/Utility.hpp"
#include <set>

namespace vk
{
	Device::Device(const std::shared_ptr<PhysicalDevice>& physicalDevice) : m_physicalDevice(physicalDevice)
	{
		std::set<uint32_t> uniqueQueueFamilies = {
			m_physicalDevice->getQueueFamilyIndices().graphicsFamily.value(),
			m_physicalDevice->getQueueFamilyIndices().presentFamily.value()
		};
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = queueFamily,
				.queueCount = 1,
				.pQueuePriorities = &queuePriority
			};
			queueCreateInfos.push_back(queueCreateInfo);
		}
		VkPhysicalDeviceFeatures deviceFeatures{};
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		VkDeviceCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
			.pQueueCreateInfos = queueCreateInfos.data(),
			.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
			.ppEnabledExtensionNames = deviceExtensions.data(),
			.pEnabledFeatures = &deviceFeatures
		};
		check(vkCreateDevice(*m_physicalDevice, &createInfo, nullptr, &m_handle));
		vkGetDeviceQueue(m_handle, m_physicalDevice->getQueueFamilyIndices().graphicsFamily.value(), 0, &m_graphicsQueue);
		vkGetDeviceQueue(m_handle, m_physicalDevice->getQueueFamilyIndices().presentFamily.value(), 0, &m_presentQueue);
	}

	Device::Device(Device&& other) noexcept :
		m_handle(other.m_handle), m_graphicsQueue(other.m_graphicsQueue),
		m_presentQueue(other.m_presentQueue), m_physicalDevice(std::move(other.m_physicalDevice))
	{
		other.m_handle = VK_NULL_HANDLE;
		other.m_graphicsQueue = VK_NULL_HANDLE;
		other.m_presentQueue = VK_NULL_HANDLE;
	}

	Device& Device::operator=(Device&& other) noexcept
	{
		m_handle = other.m_handle;
		m_graphicsQueue = other.m_graphicsQueue;
		m_presentQueue = other.m_presentQueue;
		m_physicalDevice = std::move(other.m_physicalDevice);
		other.m_handle = VK_NULL_HANDLE;
		other.m_graphicsQueue = VK_NULL_HANDLE;
		other.m_presentQueue = VK_NULL_HANDLE;
		return *this;
	}

	Device::~Device()
	{
		if (m_handle != VK_NULL_HANDLE)
		{
			vkDestroyDevice(m_handle, nullptr);
		}
	}

	VkDevice Device::get() const
	{
		return m_handle;
	}

	Device::operator VkDevice() const
	{
		return m_handle;
	}

	VkQueue Device::getGraphicsQueue() const
	{
		return m_graphicsQueue;
	}

	VkQueue Device::getPresentQueue() const
	{
		return m_presentQueue;
	}

	const std::shared_ptr<PhysicalDevice>& Device::getPhysicalDevice() const
	{
		return m_physicalDevice;
	}
}