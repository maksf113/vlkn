#include "vulkan/memory/IndexBuffer.hpp"

namespace vk
{
	IndexBuffer::IndexBuffer(const std::vector<uint32_t>& indices, const std::shared_ptr<Device>& device)
		: m_device(device), m_count(static_cast<uint32_t>(indices.size()))
	{
		VkBufferCreateInfo indexBufferCreateInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = sizeof(indices[0]) * indices.size(),
			.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE
		};
		check(vkCreateBuffer(*m_device, &indexBufferCreateInfo, nullptr, &m_handle));
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(*m_device, m_handle, &memRequirements);

		VkMemoryAllocateInfo indexMemoryAllocInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = memRequirements.size,
			.memoryTypeIndex = m_device->getPhysicalDevice()->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
		};

		check(vkAllocateMemory(*m_device, &indexMemoryAllocInfo, nullptr, &m_bufferMemory));
		check(vkBindBufferMemory(*m_device, m_handle, m_bufferMemory, 0));
		void* data;
		check(vkMapMemory(*m_device, m_bufferMemory, 0, indexBufferCreateInfo.size, 0, &data));
		memcpy(data, indices.data(), static_cast<size_t>(indexBufferCreateInfo.size));
		vkUnmapMemory(*m_device, m_bufferMemory);
	}

	IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept :
		m_handle(other.m_handle), m_bufferMemory(other.m_bufferMemory), m_device(std::move(other.m_device))
	{
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