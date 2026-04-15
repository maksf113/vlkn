#pragma once
#include "Vulkan/Core/Surface.hpp"
#include "Vulkan/Core/PhysicalDevice.hpp"
#include "Vulkan/Core/Device.hpp"
#include "Vulkan/Core/RenderPass.hpp"
#include "Window/Window.hpp"

#include <Vulkan/vulkan.h>

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
		std::vector<VkFramebuffer> m_framebufferHandles;

	public:
		SwapChain(const std::shared_ptr<Device>& device, const std::shared_ptr<Surface>& surface, const std::shared_ptr<Window>& window);
		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;
		SwapChain(SwapChain&& other) noexcept;
		SwapChain& operator=(SwapChain&& other) noexcept;
		~SwapChain();
		void createFramebuffers(const std::unique_ptr<RenderPass>& renderPass);
		void recreate(const std::shared_ptr<Window>& window, const std::unique_ptr<RenderPass>& renderPass);
		VkSwapchainKHR get() const;
		operator VkSwapchainKHR() const;
		VkFormat getImageFormat() const;

		const std::vector<VkFramebuffer>& getFramebufferHandles() const;
		VkExtent2D getExtent() const;

	private:
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const std::shared_ptr<Window>& window);
		void createImageViews();
		void clearFramebuffers();
		void clearImageViews();
	};
}