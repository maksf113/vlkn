#include "vulkan/memory/Utility.hpp"
#include "vulkan/Utility.hpp"

namespace vk
{
	void createBuffer(const std::shared_ptr<Device>& device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkBuffer* buffer, VkDeviceMemory* deviceMemory, const VkAllocationCallbacks* allocator) {
		VkBufferCreateInfo bufferCreateInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = size,
			.usage = usageFlags,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE
		};
		check(vkCreateBuffer(*device, &bufferCreateInfo, allocator, buffer));
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(*device, *buffer, &memRequirements);
		VkMemoryAllocateInfo allocInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = memRequirements.size,
			.memoryTypeIndex = device->getPhysicalDevice()->findMemoryType(memRequirements.memoryTypeBits, propertyFlags)
		};
		check(vkAllocateMemory(*device, &allocInfo, allocator, deviceMemory));
		vkBindBufferMemory(*device, *buffer, *deviceMemory, 0);
	}

	void copyBuffer(const std::shared_ptr<Device>& device, VkCommandPool commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBufferAllocateInfo allocInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = commandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1
		};
		VkCommandBuffer commandBuffer;
		check(vkAllocateCommandBuffers(*device, &allocInfo, &commandBuffer));
		VkCommandBufferBeginInfo beginInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
		};
		check(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		VkBufferCopy copyRegion{
			.srcOffset = 0,
			.dstOffset = 0,
			.size = size
		};
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
		check(vkEndCommandBuffer(commandBuffer));
		VkSubmitInfo submitInfo{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.commandBufferCount = 1,
			.pCommandBuffers = &commandBuffer
		};
		check(vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE));
		vkQueueWaitIdle(device->getGraphicsQueue());
		vkFreeCommandBuffers(*device, commandPool, 1, &commandBuffer);
	}

	void deleteBuffer(const std::shared_ptr<Device>& device, VkBuffer buffer, VkDeviceMemory deviceMemory, const VkAllocationCallbacks* allocator)
	{
		vkDestroyBuffer(*device, buffer, allocator);
		vkFreeMemory(*device, deviceMemory, allocator);
	}
}