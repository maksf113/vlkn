#include "Vulkan/Renderer/RenderPass.hpp"

#include <stdexcept>


namespace vk
{
	RenderPass::RenderPass(const std::shared_ptr<Device>& device, VkFormat format) : m_device(device)
	{
		VkAttachmentDescription colorAttachment{
			.format = format,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		};
		VkAttachmentReference colorAttachmentRef{
			.attachment = 0,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};
		VkSubpassDescription subpass{
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colorAttachmentRef
		};
		VkRenderPassCreateInfo renderPassInfo{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.attachmentCount = 1,
			.pAttachments = &colorAttachment,
			.subpassCount = 1,
			.pSubpasses = &subpass
		};
		
		if (vkCreateRenderPass(*device, &renderPassInfo, nullptr, &m_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create render pass");
		}
		
	}

	RenderPass::RenderPass(RenderPass&& other) noexcept :
		m_handle(other.m_handle), m_device(std::move(other.m_device))
	{
		other.m_handle = VK_NULL_HANDLE;
	}

	RenderPass& RenderPass::operator=(RenderPass&& other) noexcept
	{
		if(m_handle != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(*m_device, m_handle, nullptr);
		}
		m_handle = other.m_handle;
		m_device = std::move(other.m_device);
		other.m_handle = VK_NULL_HANDLE;
		return *this;
	}

	RenderPass::~RenderPass()
	{
		if (m_handle != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(*m_device, m_handle, nullptr);
		}
	}

	VkRenderPass RenderPass::get() const
	{
		return m_handle;
	}	

	RenderPass::operator VkRenderPass() const
	{
		return m_handle;
	}
}