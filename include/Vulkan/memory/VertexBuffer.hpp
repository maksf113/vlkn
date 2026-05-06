#pragma once
#include "vulkan/memory/Utility.hpp"
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
		VertexBuffer(const std::vector<Vertex>& vertices, VkMemoryPropertyFlagBits flags, const std::shared_ptr<Device>& device, VkCommandPool commandPool);
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;
		VertexBuffer(VertexBuffer&& other) noexcept;
		VertexBuffer& operator=(VertexBuffer&& other) noexcept;
		~VertexBuffer();
		VkBuffer get() const;
		operator VkBuffer() const;
		void updateToNotCoherentMemory();

		void bind(VkCommandBuffer commandBuffer) const;
	private:
		
	};

	template<typename Vertex>
	VertexBuffer<Vertex>::VertexBuffer(const std::vector<Vertex>& vertices, VkMemoryPropertyFlagBits flags, const std::shared_ptr<Device>& device, VkCommandPool commandPool) : m_device(device)
	{
		// staging buffer
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();

		// create staging buffer
		createBuffer(m_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, static_cast<VkMemoryPropertyFlagBits>(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), &stagingBuffer, &stagingBufferMemory);

		void* data;
		check(vkMapMemory(*m_device, stagingBufferMemory, 0, bufferSize, 0, &data));
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));

		// create actual vertex buffer
		createBuffer(m_device, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, flags, &m_handle, &m_bufferMemory);

		copyBuffer(m_device, commandPool, stagingBuffer, m_handle, bufferSize);

		vkUnmapMemory(*m_device, stagingBufferMemory);
		vkDestroyBuffer(*m_device, stagingBuffer, nullptr);
		vkFreeMemory(*m_device, stagingBufferMemory, nullptr);
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