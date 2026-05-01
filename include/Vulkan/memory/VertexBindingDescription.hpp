#pragma once
#include <Vulkan/vulkan.h>

namespace vk
{
	template<typename T>
	class VertexBindingDescription
	{
	private:
		VkVertexInputBindingDescription m_vertexBindingDescription;
	public:
		VertexBindingDescription(uint32_t binding, VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);
		VkVertexInputBindingDescription get() const;
	};

	template<typename T>
	VertexBindingDescription<T>::VertexBindingDescription(uint32_t binding, VkVertexInputRate inputRate)
	{
		m_vertexBindingDescription = {
			.binding = binding,
			.stride = sizeof(T),
			.inputRate = inputRate
		};
	}

	template<typename T>
	VkVertexInputBindingDescription VertexBindingDescription<T>::get() const
	{
		return m_vertexBindingDescription;
	}
}