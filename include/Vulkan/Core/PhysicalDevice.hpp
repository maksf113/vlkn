#pragma once
#include "Vulkan/Core/Instance.hpp"
#include "Vulkan/Core/Surface.hpp"

#include <Vulkan/vulkan.h>

#include <optional>
#include <vector>

namespace vk::core
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		bool isComplete() const
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	class PhysicalDevice
	{
	private:
		VkPhysicalDevice m_handle = VK_NULL_HANDLE;
		QueueFamilyIndices m_queueFamilyIndices;
		VkPhysicalDeviceProperties m_properties;
		VkPhysicalDeviceFeatures m_features;

	public:
		PhysicalDevice(const std::shared_ptr<Instance>& instance, const std::unique_ptr<Surface>& surface);
		PhysicalDevice(const PhysicalDevice&) = delete;
		PhysicalDevice& operator=(const PhysicalDevice&) = delete;
		PhysicalDevice(PhysicalDevice&& other) noexcept;
		PhysicalDevice& operator=(PhysicalDevice&& other) noexcept;
		~PhysicalDevice() = default;

		VkPhysicalDevice get() const;
		operator VkPhysicalDevice() const;

		const QueueFamilyIndices& getQueueFamilyIndices() const;
		const VkPhysicalDeviceProperties& getProperties() const;
		const VkPhysicalDeviceFeatures& getFeatures() const;

	private:
		void pickPhysicalDevice(const std::shared_ptr<Instance>& instance, const std::unique_ptr<Surface>& surface);
		bool isDeviceSuitable(VkPhysicalDevice device, const std::unique_ptr<Surface>& surface);
		bool isDeviceDiscreteGpu(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, const std::unique_ptr<Surface>& surface);
	};
}