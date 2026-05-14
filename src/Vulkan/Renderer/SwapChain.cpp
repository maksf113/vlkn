#include "vulkan/renderer/SwapChain.hpp"

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
		createDepthResources();
	}

	SwapChain::SwapChain(SwapChain&& other) noexcept :
		m_handle(other.m_handle), m_imageFormat(other.m_imageFormat), m_extent(other.m_extent),
		m_device(std::move(other.m_device)), m_surface(std::move(other.m_surface)),
		m_imageHandles(std::move(other.m_imageHandles)), m_imageViewHandles(std::move(other.m_imageViewHandles))
	{
		other.m_handle = VK_NULL_HANDLE;
	}

	SwapChain& SwapChain::operator=(SwapChain&& other) noexcept
	{
		if(m_handle != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(*m_device, m_handle, nullptr);
		}	
		m_handle = other.m_handle;
		m_imageFormat = other.m_imageFormat;
		m_extent = other.m_extent;
		m_device = std::move(other.m_device);
		m_surface = std::move(other.m_surface);
		m_imageHandles = std::move(other.m_imageHandles);
		m_imageViewHandles = std::move(other.m_imageViewHandles);
		other.m_handle = VK_NULL_HANDLE;
		return *this;
	}

	SwapChain::~SwapChain()
	{
		clearImageViews();
		if (m_handle != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(*m_device, m_handle, nullptr);
		}
	}

	void SwapChain::recreate(const std::shared_ptr<Window>& window)
	{
		vkDeviceWaitIdle(*m_device);
		clearImageViews();
		vkDestroySwapchainKHR(*m_device, m_handle, nullptr);
		m_handle = VK_NULL_HANDLE;
		SwapChain newSwapChain(m_device, m_surface, window);
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

	std::vector<VkImageView> SwapChain::getImageViewHandles() const
	{
		return m_imageViewHandles;
	}

	std::vector<VkImage> SwapChain::getImageHandles() const
	{
		return m_imageHandles;
	}

	VkExtent2D SwapChain::getExtent() const
	{
		return m_extent;
	}

	VkFormat SwapChain::getDepthFormat() const 
	{ 
		return m_depthFormat; 
	}
	VkImageView SwapChain::getDepthImageView() const 
	{
		return m_depthImageView;
	}
	VkImage SwapChain::getDepthImage() const
	{
		return m_depthImage;
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

	void SwapChain::clearImageViews()
	{
		if(m_depthImageView != VK_NULL_HANDLE)
		{
			vkDestroyImageView(*m_device, m_depthImageView, nullptr);
			vkDestroyImage(*m_device, m_depthImage, nullptr);
			vkFreeMemory(*m_device, m_depthImageMemory, nullptr);
			m_depthImageView = VK_NULL_HANDLE;
			m_depthImage = VK_NULL_HANDLE;
			m_depthImageMemory = VK_NULL_HANDLE;
		}
		for (const auto& imageView : m_imageViewHandles)
		{
			vkDestroyImageView(*m_device, imageView, nullptr);
		}
		m_imageViewHandles.clear();
	}

	void SwapChain::createDepthResources()
	{
		m_depthFormat = m_device->getPhysicalDevice()->findSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
		
		VkImageCreateInfo depthImageInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.flags = 0,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = m_depthFormat,
			.extent = { m_extent.width, m_extent.height, 1 },
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED 
		};
			
			if(vkCreateImage(*m_device, &depthImageInfo, nullptr, &m_depthImage) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create depth image");
			}

			VkMemoryRequirements memRequirements;
			vkGetImageMemoryRequirements(*m_device, m_depthImage, &memRequirements);

			VkMemoryAllocateInfo allocInfo{
				.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
				.allocationSize = memRequirements.size,
				.memoryTypeIndex = m_device->getPhysicalDevice()->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
			};

			if(vkAllocateMemory(*m_device, &allocInfo, nullptr, &m_depthImageMemory) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to allocate depth image memory");
			}
			vkBindImageMemory(*m_device, m_depthImage, m_depthImageMemory, 0);	

			VkImageViewCreateInfo depthImageViewInfo{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = m_depthImage,
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = m_depthFormat,
				//.components = {
				//	.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				//	.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				//	.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				//	.a = VK_COMPONENT_SWIZZLE_IDENTITY
				//},
				.subresourceRange = {
					.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1
				}
			};
			if(vkCreateImageView(*m_device, &depthImageViewInfo, nullptr, &m_depthImageView) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create depth image view");
			}
		
	}
}