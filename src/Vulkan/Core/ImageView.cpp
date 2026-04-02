#include "Vulkan/Core/ImageView.hpp"

#include <stdexcept>
#include <memory>


namespace vk::core
{
	ImageView::ImageView(const std::shared_ptr<Device>& device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) : 
		m_device(device), m_format(format)
	{
		VkImageViewCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = image,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = format,
			.components = {
				.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.a = VK_COMPONENT_SWIZZLE_IDENTITY
			},
			.subresourceRange = {
				.aspectMask = aspectFlags,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};
		if (vkCreateImageView(*m_device, &createInfo, nullptr, &m_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image view");
		}
	}

	ImageView::ImageView(ImageView&& other) noexcept :
		m_handle(other.m_handle), m_device(std::move(other.m_device))
	{
		other.m_handle = VK_NULL_HANDLE;
	}

	ImageView& ImageView::operator=(ImageView&& other) noexcept
	{
		m_handle = other.m_handle;
		m_device = std::move(other.m_device);
		other.m_handle = VK_NULL_HANDLE;
		return *this;
	}

	ImageView::~ImageView()
	{
		if (m_handle != VK_NULL_HANDLE)
		{
			vkDestroyImageView(*m_device, m_handle, nullptr);
		}
	}

	VkImageView ImageView::get() const
	{
		return m_handle;
	}

	ImageView::operator VkImageView() const
	{
		return m_handle;
	}
}