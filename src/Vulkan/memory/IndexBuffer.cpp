#include "vulkan/memory/IndexBuffer.hpp"
#include "vulkan/memory/Utility.hpp"

namespace vk
{
	IndexBuffer::IndexBuffer(const std::vector<uint32_t>& indices, const std::shared_ptr<Device>& device, VkCommandPool commandPool)
		: m_device(device), m_count(static_cast<uint32_t>(indices.size()))
	{
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();	

		// staging buffer
		createBuffer(m_device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

		void* data;
		check(vkMapMemory(*m_device, stagingBufferMemory, 0, bufferSize, 0, &data));
		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));

		// actual buffer
		createBuffer(m_device, bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &m_handle, &m_bufferMemory);

		// copy from staging buffer to index buffer
		copyBuffer(m_device, commandPool, stagingBuffer, m_handle, bufferSize);

		vkUnmapMemory(*m_device, stagingBufferMemory);
		deleteBuffer(m_device, stagingBuffer, stagingBufferMemory, nullptr);
	}

	IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept :
		m_handle(other.m_handle), m_bufferMemory(other.m_bufferMemory), m_device(std::move(other.m_device))
	{
		if(m_handle != VK_NULL_HANDLE)
		other.m_handle = VK_NULL_HANDLE;
		other.m_bufferMemory = VK_NULL_HANDLE;
	}

	IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) noexcept
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

	IndexBuffer::~IndexBuffer()
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

	void IndexBuffer::bind(VkCommandBuffer commandBuffer) const
	{
		vkCmdBindIndexBuffer(commandBuffer, m_handle, 0, VK_INDEX_TYPE_UINT32);
	}

	uint32_t IndexBuffer::count() const
	{
		return m_count;
	}

	VkBuffer IndexBuffer::get() const
	{
		return m_handle;
	}
}