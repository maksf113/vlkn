#pragma once
#include "Vulkan/Core/Device.hpp"

#include <Vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace vk::core
{
	class ImageView
	{
	private:
		VkImageView m_handle = VK_NULL_HANDLE;
		VkFormat m_format;
		std::shared_ptr<Device> m_device;
	public:
		ImageView(const std::shared_ptr<Device>& device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
		ImageView(const ImageView&) = delete;
		ImageView& operator=(const ImageView&) = delete;
		ImageView(ImageView&& other) noexcept;
		ImageView& operator=(ImageView&& other) noexcept;
		~ImageView();
		VkImageView get() const;
		operator VkImageView() const;
	};
}