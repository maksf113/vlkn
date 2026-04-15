#include "Application.hpp"
#include "Vulkan/Core/Instance.hpp"
#include "Vulkan/Utility.hpp"

#include <Vulkan/vulkan.h>
#include "Vulkan/vk_enum_string_helper.h"

#include <memory>

Application::Application(std::string_view name, uint16_t width, uint16_t height) :
	m_name(name)
{
	m_window = std::make_shared<Window>(width, height, name);
	m_vkContext = std::make_unique<vk::Context>(name, m_window);
	m_swapChain = std::make_shared<vk::SwapChain>(m_vkContext->getDevice(), m_vkContext->getSurface(), m_vkContext->getWindow());
	m_renderPass = std::make_unique<vk::RenderPass>(m_vkContext->getDevice(), m_swapChain->getImageFormat());
	m_swapChain->createFramebuffers(m_renderPass);

	// frame data
	for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		// command pool
		VkCommandPoolCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = m_vkContext->getDevice()->getPhysicalDevice()->getQueueFamilyIndices().graphicsFamily.value()
		};
		vk::check(vkCreateCommandPool(*m_vkContext->getDevice(), &createInfo, nullptr, &m_frames[i].commandPool));

		// command buffers
		VkCommandBufferAllocateInfo allocInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = nullptr,
			.commandPool = m_frames[i].commandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1
		};
		vk::check(vkAllocateCommandBuffers(*m_vkContext->getDevice(), &allocInfo, &m_frames[i].commandBuffer));

		// semaphores
		VkSemaphoreCreateInfo semaphoreInfo{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
		};
		
		vk::check(vkCreateSemaphore(*m_vkContext->getDevice(), &semaphoreInfo, nullptr, &m_frames[i].imageAvailableSemaphore));
		vk::check(vkCreateSemaphore(*m_vkContext->getDevice(), &semaphoreInfo, nullptr, &m_frames[i].renderFinishedSemaphore));

		// fence
		VkFenceCreateInfo fenceInfo{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT
		};
		vk::check(vkCreateFence(*m_vkContext->getDevice(), &fenceInfo, nullptr, &m_frames[i].inFlightFence));
	}
}

Application::~Application()
{
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroyFence(*m_vkContext->getDevice(), m_frames[i].inFlightFence, nullptr);
		vkDestroySemaphore(*m_vkContext->getDevice(), m_frames[i].renderFinishedSemaphore, nullptr);
		vkDestroySemaphore(*m_vkContext->getDevice(), m_frames[i].imageAvailableSemaphore, nullptr);
		vkDestroyCommandPool(*m_vkContext->getDevice(), m_frames[i].commandPool, nullptr);
	}
}

void Application::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
	};
	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to begin recording command buffer!");
	}

	// clear color
	VkClearValue clearColor = { {{0.01f, 0.01f, 0.03f, 1.0f}} };
	VkRenderPassBeginInfo renderPassInfo{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = m_renderPass->get(),
		.framebuffer = m_swapChain->getFramebufferHandles()[imageIndex],
		.renderArea = {
			.offset = { 0, 0 },
			.extent = m_swapChain->getExtent()
		},
		.clearValueCount = 1,
		.pClearValues = &clearColor
	};
	
	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	
	// pipeline

	vkCmdEndRenderPass(commandBuffer);
	vk::check(vkEndCommandBuffer(commandBuffer));
}

void Application::run()
{
	// Do host application stuff
	// Wait for the render fence
	// Acquire swapchain image and signled the swapchain image semaphore
	// Push data buffers to GPU
	// Record command buffers
	// Submit command buffers waiting for swapchain image semaphore, signaling render finish semaphore and signaling render fence
	// Present swapchain image waiting for render finish semaphore.
	while (!m_window->shouldClose())
	{
		// Do host application stuff
		m_window->pollEvents();

		// frame data
		FrameData& frame = m_frames[m_currentFrameIndex];

		// Wait for the render fence
		vk::check(vkWaitForFences(*m_vkContext->getDevice(), 1, &frame.inFlightFence, VK_TRUE, UINT64_MAX));

		// Acquire swapchain image and signled the swapchain image semaphore
		uint32_t imageIndex;
		int width = 0;
		int height = 0;
		glfwGetFramebufferSize(m_vkContext->getWindow()->get(), &width, &height);
		VkResult acquireResult = vkAcquireNextImageKHR(m_vkContext->getDevice()->get(), m_swapChain->get(), UINT64_MAX, frame.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

		std::cout << "Aquire result: " << string_VkResult(acquireResult) << "\n";
		if (m_vkContext->getWindow()->getWidth() != width || m_vkContext->getWindow()->getHeight() != height)
		{
			while (width == 0 || height == 0)
			{
				glfwGetFramebufferSize(m_vkContext->getWindow()->get(), &width, &height);
				glfwWaitEvents();
			}
			m_vkContext->getWindow()->setWidth(width);
			m_vkContext->getWindow()->setHeight(height);
			std::cout << "wdfwedf";
			m_swapChain->recreate(m_vkContext->getWindow(), m_renderPass);
			continue;

		}
		if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
		{
			std::cout << "VK_ERROR_OUT_OF_DATE_KHR";
			while (width == 0 || height == 0)
			{
				glfwGetFramebufferSize(m_vkContext->getWindow()->get(), &width, &height);
				glfwWaitEvents();
			}
			m_swapChain->recreate(m_vkContext->getWindow(), m_renderPass);
			continue;
		}
		vk::check(vkResetFences(*m_vkContext->getDevice(), 1, &frame.inFlightFence));

		vkResetCommandPool(*m_vkContext->getDevice(), frame.commandPool, 0);	
		recordCommandBuffer(frame.commandBuffer, imageIndex);
		// Submit command buffers waiting for swapchain image semaphore, signaling render finish semaphore and signaling render fence
		
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { frame.imageAvailableSemaphore	 };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &frame.commandBuffer;

		VkSemaphore signalSemaphores[] = { frame.renderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(m_vkContext->getDevice()->getGraphicsQueue(), 1, &submitInfo, frame.inFlightFence) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submit draw command buffer!");
		}

		// Present swapchain image waiting for render finish semaphore.
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { m_swapChain->get() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(m_vkContext->getDevice()->getPresentQueue(), &presentInfo);

		// next frame index
		m_currentFrameIndex = (m_currentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	}
}
