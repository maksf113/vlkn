#pragma once
#include "vulkan/core/Device.hpp"
#include "vulkan/memory/VertexBuffer.hpp"
#include "vulkan/memory/IndexBuffer.hpp"

#include <memory>

namespace vk
{
	struct Vertex
	{
		float position[3];
		float normal[3];
		float tangent[3];
		float uv[2];
	};

	class Mesh
	{
	private:
		std::unique_ptr<VertexBuffer<Vertex>> m_vertexBuffer;
		std::unique_ptr<IndexBuffer> m_indexBuffer;
	public:
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const std::shared_ptr<Device>& device, VkCommandPool commandPool);
		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;
		Mesh(Mesh&& other) noexcept = default;
		Mesh& operator=(Mesh&& other) noexcept = default;
		~Mesh() = default;
		void bind(VkCommandBuffer commandBuffer) const;
		uint32_t getIndexCount() const;
	};
}