#include "Application.hpp"
#include "Vulkan/Core/Instance.hpp"
#include "Vulkan/Core/Utility.hpp"

#include <Vulkan/vulkan.h>
#include "Vulkan/vk_enum_string_helper.h"

#include <memory>

Application::Application(std::string_view name, uint16_t width, uint16_t height) :
	m_name(name)
{
	m_window = std::make_shared<Window>(width, height, name);
	m_vkContext = std::make_unique<vk::core::Context>(name, m_window);
	m_swapChain = std::make_shared<vk::core::SwapChain>(m_vkContext->getDevice(), m_vkContext->getSurface(), m_vkContext->getWindow());
	m_renderPass = std::make_unique<vk::core::RenderPass>(m_vkContext->getDevice(), m_swapChain->getImageFormat());
	m_swapChain->createFramebuffers(m_renderPass);
	m_commandPool = std::make_unique<vk::core::CommandPool>(m_vkContext->getDevice());

	// command buffers
	VkCommandBufferAllocateInfo allocInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = *m_commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = MAX_FRAMES_IN_FLIGHT
	};
	m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateCommandBuffers(*m_vkContext->getDevice(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate command buffers!");
	}
	
	// semaphores and fences
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		m_imageAvailableSemaphores.emplace_back(m_vkContext->getDevice());
		m_renderFinishedSemaphores.emplace_back(m_vkContext->getDevice());
		m_inFlightFences.emplace_back(m_vkContext->getDevice(), true);
	}
}

Application::~Application()
{

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
		.framebuffer = m_swapChain->getFramebuffers()[imageIndex].get(),
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
	vk::core::check(vkEndCommandBuffer(commandBuffer));
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

		// Wait for the render fence
		m_inFlightFences[m_currentFrame].wait();

		// Acquire swapchain image and signled the swapchain image semaphore
		uint32_t imageIndex;
		int width = 0;
		int height = 0;
		glfwGetFramebufferSize(m_vkContext->getWindow()->get(), &width, &height);
		VkResult acquireResult = vkAcquireNextImageKHR(m_vkContext->getDevice()->get(), m_swapChain->get(), UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame].get(), VK_NULL_HANDLE, &imageIndex);
		if (acquireResult != VK_SUCCESS &&
			acquireResult != VK_NOT_READY &&
			acquireResult != VK_SUBOPTIMAL_KHR &&
			acquireResult != VK_TIMEOUT &&
			acquireResult != VK_ERROR_OUT_OF_DATE_KHR)
		{
			
		}
		std::cout << "Aquire result: " << string_VkResult(acquireResult) << "\n";
		if (m_vkContext->getWindow()->width() != width || m_vkContext->getWindow()->height() != height)
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
		m_inFlightFences[m_currentFrame].reset();
		/*
		VK_SUCCESS
		VK_NOT_READY
		VK_SUBOPTIMAL_KHR
		VK_TIMEOUT

		VK_ERROR_OUT_OF_DATE_KHR - wait for swapchain recreation - window resize
		*/


		// Push data buffers to GPU

		
		// Record command buffers
		
		vkResetCommandBuffer(m_commandBuffers[m_currentFrame], 0);
		recordCommandBuffer(m_commandBuffers[m_currentFrame], imageIndex);
		// Submit command buffers waiting for swapchain image semaphore, signaling render finish semaphore and signaling render fence
		
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame].get() };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];

		VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame].get() };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(m_vkContext->getDevice()->getGraphicsQueue(), 1, &submitInfo, m_inFlightFences[m_currentFrame].get()) != VK_SUCCESS)
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
		m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

		
	}
}
