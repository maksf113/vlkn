#include "vulkan/renderer/Renderer.hpp"
#include "vulkan/memory/VertexBindingDescription.hpp"
#include "vulkan/memory/VertexAttributeDescription.hpp"
#include "vulkan/Utility.hpp"

#include "window/GlfwInstance.hpp"

#include "vulkan/vk_enum_string_helper.h"

namespace vk
{
	Renderer::Renderer(const std::shared_ptr<Context>& context) : m_context(context)
	{
		m_swapChain = std::make_unique<vk::SwapChain>(m_context->getDevice(), m_context->getSurface(), m_context->getWindow());

		// frame data
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			// command pool
			VkCommandPoolCreateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
				.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
				.queueFamilyIndex = m_context->getDevice()->getPhysicalDevice()->getQueueFamilyIndices().graphicsFamily.value()
			};
			vk::check(vkCreateCommandPool(*m_context->getDevice(), &createInfo, nullptr, &m_frames[i].commandPool));

			// command buffers
			VkCommandBufferAllocateInfo allocInfo{
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				.pNext = nullptr,
				.commandPool = m_frames[i].commandPool,
				.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				.commandBufferCount = 1
			};
			vk::check(vkAllocateCommandBuffers(*m_context->getDevice(), &allocInfo, &m_frames[i].commandBuffer));

			// semaphores
			VkSemaphoreCreateInfo semaphoreInfo{
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
			};

			vk::check(vkCreateSemaphore(*m_context->getDevice(), &semaphoreInfo, nullptr, &m_frames[i].imageAvailableSemaphore));
			vk::check(vkCreateSemaphore(*m_context->getDevice(), &semaphoreInfo, nullptr, &m_frames[i].renderFinishedSemaphore));

			// fence
			VkFenceCreateInfo fenceInfo{
				.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				.flags = VK_FENCE_CREATE_SIGNALED_BIT
			};
			vk::check(vkCreateFence(*m_context->getDevice(), &fenceInfo, nullptr, &m_frames[i].inFlightFence));
		}

		// pipeline layout
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		check(vkCreatePipelineLayout(*m_context->getDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout));

		// pipeline config
		PipelineConfig pipelineConfig{};
		Pipeline::defaultPipelineConfig(pipelineConfig);
		pipelineConfig.addShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "C:/Dev/C++/Vulkan/shaders/triangle.vert.spv");
		pipelineConfig.addShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "C:/Dev/C++/Vulkan/shaders/triangle.frag.spv");
		pipelineConfig.pipelineLayout = m_pipelineLayout;

		pipelineConfig.rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_LINE;

		// vertex descriptors
		VertexBindingDescription<Vertex> vertexBindingDescription(0);
		VertexAttributeDescription vertexAttributeDescription(0);
		vertexAttributeDescription.pushAttribute<float>(3); // position	
		vertexAttributeDescription.pushAttribute<float>(3); // color


		pipelineConfig.vertexBindingDescriptions.push_back(vertexBindingDescription.get());
		pipelineConfig.vertexAttributeDescriptions.push_back(vertexAttributeDescription[0]);
		pipelineConfig.vertexAttributeDescriptions.push_back(vertexAttributeDescription[1]);

		pipelineConfig.vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(pipelineConfig.vertexBindingDescriptions.size());
		pipelineConfig.vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(pipelineConfig.vertexAttributeDescriptions.size());
		pipelineConfig.vertexInputStateCreateInfo.pVertexBindingDescriptions = pipelineConfig.vertexBindingDescriptions.data();
		pipelineConfig.vertexInputStateCreateInfo.pVertexAttributeDescriptions = pipelineConfig.vertexAttributeDescriptions.data();

		m_pipeline = std::make_unique<Pipeline>(m_context, pipelineConfig, m_swapChain->getImageFormat());

		// vertex buffer
		m_vertices = {
			// positions         // colors
			{-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f },
			{ 0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f },
			{-0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f },
			{ 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f }
		};
		m_vertexBuffer = std::make_unique<VertexBuffer<Vertex>>(m_context->getDevice(), m_vertices, static_cast<VkMemoryPropertyFlagBits>(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

		// index buffer
		m_indices = { 0, 1, 2, 0, 1, 3 };

		m_indexBuffer = std::make_unique<IndexBuffer>(m_indices, m_context->getDevice());
	}

	Renderer::Renderer(Renderer&& other) noexcept :
		m_context(std::move(other.m_context)), m_swapChain(std::move(other.m_swapChain)), m_currentFrameIndex(other.m_currentFrameIndex)
	{
		std::copy(std::begin(other.m_frames), std::end(other.m_frames), std::begin(m_frames));
		std::fill(std::begin(other.m_frames), std::end(other.m_frames), FrameData{});
	}

	Renderer& Renderer::operator=(Renderer&& other) noexcept
	{
		if(m_pipelineLayout != VK_NULL_HANDLE)
		{
			vkDestroyPipelineLayout(*m_context->getDevice(), m_pipelineLayout, nullptr);
		}
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroyFence(*m_context->getDevice(), m_frames[i].inFlightFence, nullptr);
			vkDestroySemaphore(*m_context->getDevice(), m_frames[i].renderFinishedSemaphore, nullptr);
			vkDestroySemaphore(*m_context->getDevice(), m_frames[i].imageAvailableSemaphore, nullptr);
			vkDestroyCommandPool(*m_context->getDevice(), m_frames[i].commandPool, nullptr);
		}
		m_context = std::move(other.m_context);
		m_swapChain = std::move(other.m_swapChain);
		m_currentFrameIndex = other.m_currentFrameIndex;
		std::copy(std::begin(other.m_frames), std::end(other.m_frames), std::begin(m_frames));
		std::fill(std::begin(other.m_frames), std::end(other.m_frames), FrameData{});
		return *this;
	}

	Renderer::~Renderer()
	{
		// 1. Destroy the pipeline layout
		if (m_pipelineLayout != VK_NULL_HANDLE)
		{
			vkDestroyPipelineLayout(*m_context->getDevice(), m_pipelineLayout, nullptr);
		}

		// 2. Destroy per-frame sync objects and command pools
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroyFence(*m_context->getDevice(), m_frames[i].inFlightFence, nullptr);
			vkDestroySemaphore(*m_context->getDevice(), m_frames[i].renderFinishedSemaphore, nullptr);
			vkDestroySemaphore(*m_context->getDevice(), m_frames[i].imageAvailableSemaphore, nullptr);
			vkDestroyCommandPool(*m_context->getDevice(), m_frames[i].commandPool, nullptr);
		}
	}

	void Renderer::drawFrame()
	{
		// frame data
		FrameData& frame = m_frames[m_currentFrameIndex];

		// Wait for the render fence
		vk::check(vkWaitForFences(*m_context->getDevice(), 1, &frame.inFlightFence, VK_TRUE, UINT64_MAX));

		// Acquire swapchain image and signled the swapchain image semaphore
		uint32_t imageIndex;
		int width = 0;
		int height = 0;
		glfwGetFramebufferSize(m_context->getWindow()->get(), &width, &height);
		VkResult acquireResult = vkAcquireNextImageKHR(m_context->getDevice()->get(), m_swapChain->get(), UINT64_MAX, frame.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

		if (m_context->getWindow()->getWidth() != width || m_context->getWindow()->getHeight() != height)
		{
			while (width == 0 || height == 0)
			{
				glfwGetFramebufferSize(m_context->getWindow()->get(), &width, &height);
				glfwWaitEvents();
			}
			m_context->getWindow()->setWidth(width);
			m_context->getWindow()->setHeight(height);
			m_swapChain->recreate(m_context->getWindow());
			return;
		}
		if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
		{
			std::cout << "VK_ERROR_OUT_OF_DATE_KHR";
			while (width == 0 || height == 0)
			{
				glfwGetFramebufferSize(m_context->getWindow()->get(), &width, &height);
				glfwWaitEvents();
			}
			m_swapChain->recreate(m_context->getWindow());
			return;
		}
		vk::check(vkResetFences(*m_context->getDevice(), 1, &frame.inFlightFence));

		vkResetCommandPool(*m_context->getDevice(), frame.commandPool, 0);
		recordDynamicCommandBuffer(frame.commandBuffer, imageIndex);
		// Submit command buffers waiting for swapchain image semaphore, signaling render finish semaphore and signaling render fence

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { frame.imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &frame.commandBuffer;

		VkSemaphore signalSemaphores[] = { frame.renderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(m_context->getDevice()->getGraphicsQueue(), 1, &submitInfo, frame.inFlightFence) != VK_SUCCESS)
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

		VkResult presentResult = vkQueuePresentKHR(m_context->getDevice()->getPresentQueue(), &presentInfo);
		if(presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR)
		{
			while (width == 0 || height == 0)
			{
				glfwGetFramebufferSize(m_context->getWindow()->get(), &width, &height);
				glfwWaitEvents();
			}
			m_swapChain->recreate(m_context->getWindow());
		}
		 else if (presentResult != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image!");
		}

		// next frame index
		m_currentFrameIndex = (m_currentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void Renderer::recordDynamicCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
		};
		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		// dynamic viewport and scissor
		VkViewport viewport{
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(m_swapChain->getExtent().width),
			.height = static_cast<float>(m_swapChain->getExtent().height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{
			.offset = { 0, 0 },
			.extent = m_swapChain->getExtent()
		};
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		VkImageSubresourceRange sub{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		};

		VkImageMemoryBarrier2 initialBarrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
			.srcAccessMask = VK_ACCESS_2_NONE,
			.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.image = m_swapChain->getImageHandles()[imageIndex],
			.subresourceRange = sub
		};



		VkDependencyInfo dependencyInfo{
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &initialBarrier
		};

		vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);

		VkRenderingAttachmentInfo colorAttachmentInfo{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = m_swapChain->getImageViewHandles()[imageIndex],
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = VkClearValue{.color = {{0.01f, 0.01f, 0.03f, 1.0f}} },
		};

		VkRenderingInfo renderingInfo{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.renderArea = {
				.offset = { 0, 0 },
				.extent = m_swapChain->getExtent()
			},
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colorAttachmentInfo,
			.pDepthAttachment = nullptr,
			.pStencilAttachment = nullptr
		};


		vkCmdBeginRendering(commandBuffer, &renderingInfo);

		// bind pipeline
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->get());
		
		// bind vertex buffer
		m_vertexBuffer->bind(commandBuffer);

		// bind index buffer
		m_indexBuffer->bind(commandBuffer);

		// draw call
		vkCmdDrawIndexed(commandBuffer, m_indexBuffer->count(), 1, 0, 0, 0);

		vkCmdEndRendering(commandBuffer);

		VkImageSubresourceRange endSub{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		};

		VkImageMemoryBarrier2 endBarrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_2_NONE,
			.dstAccessMask = VK_ACCESS_2_NONE,
			.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			.image = m_swapChain->getImageHandles()[imageIndex],
			.subresourceRange = endSub
		};



		VkDependencyInfo endDependencyInfo{
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &endBarrier
		};

		vkCmdPipelineBarrier2(commandBuffer, &endDependencyInfo);

		vk::check(vkEndCommandBuffer(commandBuffer));
	}


	void Renderer::recreateSwapChain()
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(m_context->getWindow()->get(), &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(m_context->getWindow()->get(), &width, &height);
			glfwWaitEvents();
		}
		m_swapChain->recreate(m_context->getWindow());
	}


}