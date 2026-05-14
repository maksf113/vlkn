#include "vulkan/memory/Mesh.hpp"

#include <vulkan/vulkan.h>

namespace vk
{
	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::shared_ptr<Device>& device, VkCommandPool commandPool)
	{
		m_vertexBuffer = std::make_unique<VertexBuffer<Vertex>>(vertices, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, device, commandPool);
		m_indexBuffer = std::make_unique<IndexBuffer>(indices, device, commandPool);
	}
	void Mesh::bind(VkCommandBuffer commandBuffer) const
	{
		m_vertexBuffer->bind(commandBuffer);
		m_indexBuffer->bind(commandBuffer);
	}
	uint32_t Mesh::getIndexCount() const
	{
		return m_indexBuffer->getCount();
	}
}