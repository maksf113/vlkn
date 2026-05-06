#pragma once
#include "vulkan/core/Device.hpp"

#include <Vulkan/vulkan.h>

#include <memory>

namespace vk
{
	void createBuffer(const std::shared_ptr<Device>& device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkBuffer* buffer, VkDeviceMemory* deviceMemory, const VkAllocationCallbacks* allocator = nullptr);
	void copyBuffer(const std::shared_ptr<Device>& device, VkCommandPool commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void deleteBuffer(const std::shared_ptr<Device>& device, VkBuffer buffer, VkDeviceMemory deviceMemory, const VkAllocationCallbacks* allocator = nullptr);
}