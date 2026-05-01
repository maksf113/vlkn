#pragma once
#include "vulkan/core/Device.hpp"
#include "vulkan/Utility.hpp"

#include <Vulkan/vulkan.h>

#include <vector>

namespace vk
{
	struct Vertex
	{
		float position[3];
		float color[3];
	};

	template <typename Vertex>
	class VertexBuffer
	{
	private:
		VkBuffer m_handle;
		VkDeviceMemory m_bufferMemory;
		std::shared_ptr<Device> m_device;
	public:
		VertexBuffer(const std::shared_ptr<Device>& device, const std::vector<Vertex>& vertices, VkMemoryPropertyFlagBits flags);
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;
		VertexBuffer(VertexBuffer&& other) noexcept;
		VertexBuffer& operator=(VertexBuffer&& other) noexcept;
		~VertexBuffer();
		VkBuffer get() const;
		operator VkBuffer() const;

		void bind(VkCommandBuffer commandBuffer) const;
	private:
		
	};

	template<typename Vertex>
	VertexBuffer<Vertex>::VertexBuffer(const std::shared_ptr<Device>& device, const std::vector<Vertex>& vertices, VkMemoryPropertyFlagBits flags) : m_device(device)
	{
		VkBufferCreateInfo bufferCreateInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = sizeof(Vertex) * vertices.size(),
			.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE
		};

		check(vkCreateBuffer(*m_device, &bufferCreateInfo, nullptr, &m_handle));

		// get memory requirements
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(*m_device, m_handle, &memRequirements);
		VkMemoryAllocateInfo memoryAllocInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = memRequirements.size,
			.memoryTypeIndex = m_device->getPhysicalDevice()->findMemoryType(memRequirements.memoryTypeBits, flags)
		};
		// allocate memory, bind memory and copy vertex data
		check(vkAllocateMemory(*m_device, &memoryAllocInfo, nullptr, &m_bufferMemory));
		check(vkBindBufferMemory(*m_device, m_handle, m_bufferMemory, 0));
		void* data;
		check(vkMapMemory(*m_device, m_bufferMemory, 0, bufferCreateInfo.size, 0, &data));
		memcpy(data, vertices.data(), static_cast<size_t>(bufferCreateInfo.size));
		vkUnmapMemory(*m_device, m_bufferMemory);
	}

	template<typename Vertex>
	VertexBuffer<Vertex>::VertexBuffer(VertexBuffer&& other) noexcept :
		m_handle(other.m_handle), m_bufferMemory(other.m_bufferMemory), m_device(std::move(other.m_device))
	{
		other.m_handle = VK_NULL_HANDLE;
		other.m_bufferMemory = VK_NULL_HANDLE;
	}

	template<typename Vertex>
	VertexBuffer<Vertex>& VertexBuffer<Vertex>::operator=(VertexBuffer&& other) noexcept
	{
		if (m_handle != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(*m_device, m_handle, nullptr);
		}
		if (m_bufferMemory != VK_NULL_HANDLE)
		{
			vkFreeMemory(*m_device, m_bufferMemory, nullptr);
		}
		m_handle = other.m_handle;
		m_bufferMemory = other.m_bufferMemory;
		m_device = std::move(other.m_device);
		other.m_handle = VK_NULL_HANDLE;
		other.m_bufferMemory = VK_NULL_HANDLE;
		return *this;
	}

	template<typename Vertex>
	VertexBuffer<Vertex>::~VertexBuffer()
	{
		if (m_handle != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(*m_device, m_handle, nullptr);
		}
		if (m_bufferMemory != VK_NULL_HANDLE)
		{
			vkFreeMemory(*m_device, m_bufferMemory, nullptr);
		}
	}

	template<typename Vertex>
	VkBuffer VertexBuffer<Vertex>::get() const
	{
		return m_handle;
	}

	template<typename Vertex>
	VertexBuffer<Vertex>::operator VkBuffer() const
	{
		return m_handle;
	}

	template<typename Vertex>
	void VertexBuffer<Vertex>::bind(VkCommandBuffer commandBuffer) const
	{
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_handle, offsets);
	}
}