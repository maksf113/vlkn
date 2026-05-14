#pragma once
#include "vulkan/core/Device.hpp"
#include "vulkan/Utility.hpp"

#include <Vulkan/vulkan.h>

#include <vector>
#include <memory>

namespace vk
{
	class IndexBuffer
	{
	private:
		VkBuffer m_handle;
		VkDeviceMemory m_bufferMemory;
		uint32_t m_count;
		std::shared_ptr<Device> m_device;	
	public:
		IndexBuffer(const std::vector<uint32_t>& indices, const std::shared_ptr<Device>& device, VkCommandPool commandPool);
		IndexBuffer(const IndexBuffer&) = delete;
		IndexBuffer& operator=(const IndexBuffer&) = delete;
		IndexBuffer(IndexBuffer&&) noexcept;
		IndexBuffer& operator=(IndexBuffer&&) noexcept;
		~IndexBuffer();

		void bind(VkCommandBuffer commandBuffer) const;
		uint32_t getCount() const;
		VkBuffer get() const;
	};
}