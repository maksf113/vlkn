#pragma once
#include <Vulkan/vulkan.h>

#include <vector>
#include <stdexcept>
#include <type_traits>

namespace vk
{
	class VertexAttributeDescription
	{
	private:
		std::vector<VkVertexInputAttributeDescription> m_attributeDescriptions;	
		uint32_t m_currentStride = 0;
		uint32_t m_binding;
	public:
		VertexAttributeDescription(uint32_t binding);
		VertexAttributeDescription(const VertexAttributeDescription&) = delete;
		VertexAttributeDescription& operator=(const VertexAttributeDescription&) = delete;
		VertexAttributeDescription(VertexAttributeDescription&& other) noexcept = default;
		VertexAttributeDescription& operator=(VertexAttributeDescription&& other) noexcept = default;
		~VertexAttributeDescription() = default;
		template<typename T>
		void pushAttribute(uint32_t n);
		template<typename T>
		void pushAttribute(uint32_t n, VkFormat format);
		VkVertexInputAttributeDescription operator[](size_t index) const;

	private:
		template<typename T>
		static VkFormat getFormatForType(uint32_t n);
	};

	VertexAttributeDescription::VertexAttributeDescription(uint32_t binding)
		: m_binding(binding) {}

	template<typename T>
	inline void VertexAttributeDescription::pushAttribute(uint32_t n)
	{
		VkVertexInputAttributeDescription attributeDescription{
			.location = static_cast<uint32_t>(m_attributeDescriptions.size()),
			.binding = m_binding,
			.format = getFormatForType<T>(n), 
			.offset = m_currentStride
		};
		m_currentStride += sizeof(T) * n;
		m_attributeDescriptions.push_back(attributeDescription);
	}

	template<typename T>
	inline void VertexAttributeDescription::pushAttribute(uint32_t n, VkFormat format)
	{
		VkVertexInputAttributeDescription attributeDescription{
			.location = static_cast<uint32_t>(m_attributeDescriptions.size()),
			.binding = m_binding,
			.format = format,
			.offset = m_currentStride
		};
		m_currentStride += sizeof(T) * n; // to do: sizeOfFormat(VkFormat format)
		m_attributeDescriptions.push_back(attributeDescription);
	}

	VkVertexInputAttributeDescription VertexAttributeDescription::operator[](size_t index) const
	{
		if (index >= m_attributeDescriptions.size())
			throw std::out_of_range("Attribute index out of range");
		return m_attributeDescriptions[index];
	}


	template<typename T>
	inline VkFormat VertexAttributeDescription::getFormatForType(uint32_t n)
	{
		if constexpr (std::is_same<T, float>::value)
		{
			switch (n)
			{
			case 1: return VK_FORMAT_R32_SFLOAT;
			case 2: return VK_FORMAT_R32G32_SFLOAT;
			case 3: return VK_FORMAT_R32G32B32_SFLOAT;
			case 4: return VK_FORMAT_R32G32B32A32_SFLOAT;
			default: throw std::runtime_error("Unsupported number of components for float type");
			}
		}
		else if constexpr (std::is_same<T, int>::value)
		{
			switch (n)
			{
			case 1: return VK_FORMAT_R32_SINT;
			case 2: return VK_FORMAT_R32G32_SINT;
			case 3: return VK_FORMAT_R32G32B32_SINT;
			case 4: return VK_FORMAT_R32G32B32A32_SINT;
			default: throw std::runtime_error("Unsupported number of components for int type");
			}
		}
		else
		{
			throw std::runtime_error("Unsupported attribute type");
		}
	}
}