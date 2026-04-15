#include "Vulkan/Core/SwapChain.hpp"

#include <vector>
#include <stdexcept>
#include <memory>
#include <iostream>

namespace vk
{
	SwapChain::SwapChain(const std::shared_ptr<Device>& device, const std::shared_ptr<Surface>& surface, const std::shared_ptr<Window>& window) :
		m_device(device), m_surface(surface)
	{
		SwapChainSupportDetails swapChainSupport = m_device->getPhysicalDevice()->querySwapChainSupport(surface);
		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);

		m_imageFormat = surfaceFormat.format;
		m_extent = extent;

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = *surface,
			.minImageCount = imageCount,
			.imageFormat = surfaceFormat.format,
			.imageColorSpace = surfaceFormat.colorSpace,
			.imageExtent = extent,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
		};

		const QueueFamilyIndices& indices = m_device->getPhysicalDevice()->getQueueFamilyIndices();
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
		if (indices.graphicsFamily != indices.presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(*m_device, &createInfo, nullptr, &m_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create swap chain");
		}

		vkGetSwapchainImagesKHR(*m_device, m_handle, &imageCount, nullptr);
		m_imageHandles.resize(imageCount);
		vkGetSwapchainImagesKHR(*m_device, m_handle, &imageCount, m_imageHandles.data());

		createImageViews();
	}

	SwapChain::SwapChain(SwapChain&& other) noexcept :
		m_handle(other.m_handle), m_imageFormat(other.m_imageFormat), m_extent(other.m_extent),
		m_device(std::move(other.m_device)), m_surface(std::move(other.m_surface)),
		m_imageHandles(std::move(other.m_imageHandles)), m_imageViewHandles(std::move(other.m_imageViewHandles)),
		m_framebufferHandles(std::move(other.m_framebufferHandles))
	{
		other.m_handle = VK_NULL_HANDLE;
	}

	SwapChain& SwapChain::operator=(SwapChain&& other) noexcept
	{
		m_handle = other.m_handle;
		m_imageFormat = other.m_imageFormat;
		m_extent = other.m_extent;
		m_device = std::move(other.m_device);
		m_surface = std::move(other.m_surface);
		m_imageHandles = std::move(other.m_imageHandles);
		m_imageViewHandles = std::move(other.m_imageViewHandles);
		m_framebufferHandles = std::move(other.m_framebufferHandles);	
		other.m_handle = VK_NULL_HANDLE;
		return *this;
	}

	SwapChain::~SwapChain()
	{
		clearFramebuffers();
		clearImageViews();
		if (m_handle != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(*m_device, m_handle, nullptr);
		}
	}

	void SwapChain::recreate(const std::shared_ptr<Window>& window, const std::unique_ptr<RenderPass>& renderPass)
	{
		vkDeviceWaitIdle(*m_device);
		clearFramebuffers();
		vkDestroySwapchainKHR(*m_device, m_handle, nullptr);
		m_handle = VK_NULL_HANDLE;
		SwapChain newSwapChain(m_device, m_surface, window);
		newSwapChain.createFramebuffers(renderPass);
		*this = std::move(newSwapChain);
		std::cout << "SC recreation\n";
	}

	VkSwapchainKHR SwapChain::get() const
	{
		return m_handle;
	}

	SwapChain::operator VkSwapchainKHR() const
	{
		return m_handle;
	}

	VkFormat SwapChain::getImageFormat() const
	{
		return m_imageFormat;
	}

	const std::vector<VkFramebuffer>& SwapChain::getFramebufferHandles() const
	{
		return m_framebufferHandles;
	}

	VkExtent2D SwapChain::getExtent() const
	{
		return m_extent;
	}

	VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}
		return availableFormats[0];
	}

	VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}
		// fifo always available
		return VK_PRESENT_MODE_FIFO_KHR;
	}
	
	VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const std::shared_ptr<Window>& window)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
		{
			return capabilities.currentExtent;
		}
		else
		{
			VkExtent2D actualExtent = {
				window->getWidth(),
				window->getHeight()
			};
			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
			return actualExtent;
		}
	}

	void SwapChain::createImageViews()
	{
		m_imageViewHandles.resize(m_imageHandles.size());
		for(int i = 0; i < m_imageHandles.size(); i++)
		{
			VkImageViewCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = m_imageHandles[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = m_imageFormat,
			.components = {
				.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.a = VK_COMPONENT_SWIZZLE_IDENTITY
			},
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
			};
			if (vkCreateImageView(*m_device, &createInfo, nullptr, &m_imageViewHandles[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create image view");
			}
		}
	}

	void SwapChain::createFramebuffers(const std::unique_ptr<RenderPass>& renderPass)
	{
		if (m_imageViewHandles.size() < 1)
		{
			throw std::runtime_error("Attempting to create swap chain framebuffers with no image views");
		}
		m_framebufferHandles.resize(m_imageViewHandles.size());
		for (int i = 0; i < m_imageViewHandles.size(); i++)
		{
			VkFramebufferCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = *renderPass,
			.attachmentCount = static_cast<uint32_t>(1),
			.pAttachments = &m_imageViewHandles[i],
			.width = m_extent.width,
			.height = m_extent.height,
			.layers = 1
			};
			if (vkCreateFramebuffer(*m_device, &createInfo, nullptr, &m_framebufferHandles[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create framebuffer");
			}
		}
		
	}

	void SwapChain::clearFramebuffers()
	{
		for (const auto& framebuffer : m_framebufferHandles)
		{
			vkDestroyFramebuffer(*m_device, framebuffer, nullptr);
		}
		m_framebufferHandles.clear();
	}

	void SwapChain::clearImageViews()
	{
		for (const auto& imageView : m_imageViewHandles)
		{
			vkDestroyImageView(*m_device, imageView, nullptr);
		}
		m_imageViewHandles.clear();
	}
}