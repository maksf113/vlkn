#include "vulkan/core/PhysicalDevice.hpp"
#include "vulkan/Utility.hpp"
#include <map>

namespace vk
{
	PhysicalDevice::PhysicalDevice(const std::shared_ptr<Instance>& instance, const std::shared_ptr<Surface>& surface)
	{
		pickPhysicalDevice(instance, surface);
	}
	PhysicalDevice::PhysicalDevice(PhysicalDevice&& other) noexcept :
		m_handle(other.m_handle), m_queueFamilyIndices(std::move(other.m_queueFamilyIndices)),
		m_properties(other.m_properties), m_features(other.m_features)
	{
		other.m_handle = VK_NULL_HANDLE;
	}
	PhysicalDevice& PhysicalDevice::operator=(PhysicalDevice&& other) noexcept
	{
		m_handle = other.m_handle;
		m_queueFamilyIndices = std::move(other.m_queueFamilyIndices);
		m_properties = other.m_properties;
		m_features = other.m_features;
		other.m_handle = VK_NULL_HANDLE;
		return *this;
	}
	VkPhysicalDevice PhysicalDevice::get() const
	{
		return m_handle;
	}
	PhysicalDevice::operator VkPhysicalDevice() const
	{
		return m_handle;
	}
	const QueueFamilyIndices& PhysicalDevice::getQueueFamilyIndices() const
	{
		return m_queueFamilyIndices;
	}
	const VkPhysicalDeviceProperties& PhysicalDevice::getProperties() const
	{
		return m_properties;
	}
	const VkPhysicalDeviceFeatures& PhysicalDevice::getFeatures() const
	{
		return m_features;
	}
	void PhysicalDevice::pickPhysicalDevice(const std::shared_ptr<Instance>& instance, const std::shared_ptr<Surface>& surface)
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(*instance, &deviceCount, nullptr);

		if (deviceCount == 0)
			throw std::runtime_error("Failed to find GPU with Vulkan support");

		std::vector<VkPhysicalDevice> devices(deviceCount);

		vkEnumeratePhysicalDevices(*instance, &deviceCount, devices.data());

		bool foundDiscreteGpu = false;	
		for (const auto& device : devices)
			if (isDeviceSuitable(device, surface))
			{
				m_handle = device;
				vkGetPhysicalDeviceProperties(m_handle, &m_properties);
				vkGetPhysicalDeviceFeatures(m_handle, &m_features);
				if(m_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				{
					break;
					foundDiscreteGpu = true;
				}
			}
		if(!foundDiscreteGpu)
		{
			for (const auto& device : devices)
				if (isDeviceSuitable(device, surface))
				{
					m_handle = device;
					vkGetPhysicalDeviceProperties(m_handle, &m_properties);
					vkGetPhysicalDeviceFeatures(m_handle, &m_features);
					break;
				}
		}

		if (m_handle == VK_NULL_HANDLE)
			throw std::runtime_error("Failed to find a suitable GPU");
	}

	bool PhysicalDevice::isDeviceSuitable(VkPhysicalDevice device, const std::shared_ptr<Surface>& surface)
	{
		m_queueFamilyIndices = findQueueFamilies(device, surface);
		return m_queueFamilyIndices.isComplete();
	}

	bool PhysicalDevice::isDeviceDiscreteGpu(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
	}

	QueueFamilyIndices PhysicalDevice::findQueueFamilies(VkPhysicalDevice device, const std::shared_ptr<Surface>& surface)
	{
		QueueFamilyIndices indices;
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
		for (uint32_t i = 0; i < queueFamilies.size(); i++)
		{
			if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, *surface, &presentSupport);
			if (presentSupport)
			{
				indices.presentFamily = i;
			}
			if (indices.isComplete())
			{
				break;
			}
		}
		return indices;
	}

	SwapChainSupportDetails PhysicalDevice::querySwapChainSupport(const std::shared_ptr<Surface>& surface) const
	{
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_handle, *surface, &details.capabilities);
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_handle, *surface, &formatCount, nullptr);
		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(m_handle, *surface, &formatCount, details.formats.data());
		}
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_handle, *surface, &presentModeCount, nullptr);
		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(m_handle, *surface, &presentModeCount, details.presentModes.data());
		}
		return details;
	}

	uint32_t PhysicalDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_handle, &memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
		throw std::runtime_error("Failed to find suitable memory type");
	}

	VkFormat PhysicalDevice::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const {
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(m_handle, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}
		throw std::runtime_error("Failed to find supported format!");
	}
}