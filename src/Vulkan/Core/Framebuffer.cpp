#include "Vulkan/Core/Framebuffer.hpp"

#include <stdexcept>

namespace vk::core
{
	Framebuffer::Framebuffer(const std::shared_ptr<Device>& device, VkRenderPass renderPass, const std::vector<VkImageView>& attachments, VkExtent2D extent) :
		m_device(device)
	{
		VkFramebufferCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = renderPass,
			.attachmentCount = static_cast<uint32_t>(attachments.size()),
			.pAttachments = attachments.data(),
			.width = extent.width,
			.height = extent.height,
			.layers = 1
		};
		if (vkCreateFramebuffer(*m_device, &createInfo, nullptr, &m_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create framebuffer");
		}
	}
	Framebuffer::Framebuffer(Framebuffer&& other) noexcept :
		m_handle(other.m_handle), m_device(std::move(other.m_device))
	{
		other.m_handle = VK_NULL_HANDLE;
	}
	Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
	{
		m_handle = other.m_handle;
		m_device = std::move(other.m_device);
		other.m_handle = VK_NULL_HANDLE;
		return *this;
	}
	Framebuffer::~Framebuffer()
	{
		if (m_handle != VK_NULL_HANDLE)
		{
			vkDestroyFramebuffer(*m_device, m_handle, nullptr);
		}
	}
	VkFramebuffer Framebuffer::get() const
	{
		return m_handle;
	}
	Framebuffer::operator VkFramebuffer() const
	{
		return m_handle;
	}

}