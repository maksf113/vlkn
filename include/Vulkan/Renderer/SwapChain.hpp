#pragma once
#include "vulkan/core/Surface.hpp"
#include "vulkan/core/PhysicalDevice.hpp"
#include "vulkan/core/Device.hpp"

#include "window/Window.hpp"

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace vk
{
	class SwapChain
	{
	private:
		VkSwapchainKHR m_handle = VK_NULL_HANDLE;
		VkFormat m_imageFormat;
		VkExtent2D m_extent;
		std::shared_ptr<Device> m_device;
		std::shared_ptr<Surface> m_surface;
		std::vector<VkImage> m_imageHandles;
		std::vector<VkImageView> m_imageViewHandles;

	public:
		SwapChain(const std::shared_ptr<Device>& device, const std::shared_ptr<Surface>& surface, const std::shared_ptr<Window>& window);
		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;
		SwapChain(SwapChain&& other) noexcept;
		SwapChain& operator=(SwapChain&& other) noexcept;
		~SwapChain();
		void recreate(const std::shared_ptr<Window>& window);
		VkSwapchainKHR get() const;
		operator VkSwapchainKHR() const;
		VkFormat getImageFormat() const;
		std::vector<VkImageView> getImageViewHandles() const;
		std::vector<VkImage> getImageHandles() const;
		VkExtent2D getExtent() const;

	private:
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const std::shared_ptr<Window>& window);
		void createImageViews();
		void clearImageViews();

	};
}