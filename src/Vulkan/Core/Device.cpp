#include "vulkan/core/Device.hpp"
#include "vulkan/Utility.hpp"
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

		// 1. Setup Vulkan 1.3 core features
		VkPhysicalDeviceVulkan13Features vulkan13Features{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
			.pNext = nullptr,
			.synchronization2 = VK_TRUE,
			.dynamicRendering = VK_TRUE
		};

		// 2. Wrap standard features in a Features2 struct and chain the 1.3 features
		VkPhysicalDeviceFeatures2 deviceFeatures2{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
			.pNext = &vulkan13Features,
			.features = {} // Zero initialize first
		};
		deviceFeatures2.features.fillModeNonSolid = VK_TRUE; // Enable wireframe mode

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		VkDeviceCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = &deviceFeatures2,
			.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
			.pQueueCreateInfos = queueCreateInfos.data(),
			.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
			.ppEnabledExtensionNames = deviceExtensions.data(),
			.pEnabledFeatures = nullptr // features enabled in pNext
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
		if(m_handle != VK_NULL_HANDLE)
		{
			vkDestroyDevice(m_handle, nullptr);
		}
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