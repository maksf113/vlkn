#include "vulkan/renderer/Renderer.hpp"
#include "vulkan/memory/VertexBindingDescription.hpp"
#include "vulkan/memory/VertexAttributeDescription.hpp"
#include "vulkan/Utility.hpp"

#include "window/GlfwInstance.hpp"

#include <vulkan/vk_enum_string_helper.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace vk
{
	Renderer::Renderer(const std::shared_ptr<Context>& context) : m_context(context)
	{
		m_appStartTime = std::chrono::high_resolution_clock::now();
		m_swapChain = std::make_unique<vk::SwapChain>(m_context->getDevice(), m_context->getSurface(), m_context->getWindow());

		// descriptor set layout
		// binding 0: uniform buffer (vertex shader)
		VkDescriptorSetLayoutBinding uboLayoutBinding{
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
			.pImmutableSamplers = nullptr
		};

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.bindingCount = 1,
			.pBindings = &uboLayoutBinding
		};

		vk::check(vkCreateDescriptorSetLayout(*m_context->getDevice(), &descriptorSetLayoutInfo, nullptr, &m_descriptorSetLayout));

		// pipeline layout
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1; // uniform buffer layout
		pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		check(vkCreatePipelineLayout(*m_context->getDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout));

		// pipeline config
		PipelineConfig pipelineConfig{};
		Pipeline::defaultPipelineConfig(pipelineConfig);
		pipelineConfig.addShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "C:/Dev/C++/Vulkan/shaders/triangle.vert.spv");
		pipelineConfig.addShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "C:/Dev/C++/Vulkan/shaders/triangle.frag.spv");
		pipelineConfig.pipelineLayout = m_pipelineLayout;

		pipelineConfig.rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_LINE;

		// vertex descriptions
		VertexBindingDescription<Vertex> vertexBindingDescription(0);
		VertexAttributeDescription vertexAttributeDescription(0);
		vertexAttributeDescription.pushAttribute<float>(3); // position	
		vertexAttributeDescription.pushAttribute<float>(3); // normal
		vertexAttributeDescription.pushAttribute<float>(3); // tangent
		vertexAttributeDescription.pushAttribute<float>(2); // uv

		pipelineConfig.vertexBindingDescriptions.push_back(vertexBindingDescription.get());
		pipelineConfig.vertexAttributeDescriptions.push_back(vertexAttributeDescription[0]);
		pipelineConfig.vertexAttributeDescriptions.push_back(vertexAttributeDescription[1]);
		pipelineConfig.vertexAttributeDescriptions.push_back(vertexAttributeDescription[2]);
		pipelineConfig.vertexAttributeDescriptions.push_back(vertexAttributeDescription[3]);

		pipelineConfig.vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(pipelineConfig.vertexBindingDescriptions.size());
		pipelineConfig.vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(pipelineConfig.vertexAttributeDescriptions.size());
		pipelineConfig.vertexInputStateCreateInfo.pVertexBindingDescriptions = pipelineConfig.vertexBindingDescriptions.data();
		pipelineConfig.vertexInputStateCreateInfo.pVertexAttributeDescriptions = pipelineConfig.vertexAttributeDescriptions.data();

		m_pipeline = std::make_unique<Pipeline>(m_context, pipelineConfig, m_swapChain->getImageFormat(), m_swapChain->getDepthFormat());

		// descriptor pool
		VkDescriptorPoolSize poolSize{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = MAX_FRAMES_IN_FLIGHT
		};
		VkDescriptorPoolCreateInfo descriptorPoolInfo{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = MAX_FRAMES_IN_FLIGHT,
			.poolSizeCount = 1,
			.pPoolSizes = &poolSize
		};
		vk::check(vkCreateDescriptorPool(*m_context->getDevice(), &descriptorPoolInfo, nullptr, &m_descriptorPool));

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


			// uniform buffers
			m_frames[i].uniformBuffer = std::make_unique<UniformBuffer<UniformData>>(m_context->getDevice());
			
			// descriptor sets
			VkDescriptorSetAllocateInfo allocInfoDS{
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
				.descriptorPool = m_descriptorPool,
				.descriptorSetCount = 1,
				.pSetLayouts = &m_descriptorSetLayout
			};

			check(vkAllocateDescriptorSets(*m_context->getDevice(), &allocInfoDS, &m_frames[i].descriptorSet));

			VkDescriptorBufferInfo bufferInfo{
				.buffer = *m_frames[i].uniformBuffer,
				.offset = 0,
				.range = sizeof(UniformData)
			};

			VkWriteDescriptorSet descriptorWrite{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = m_frames[i].descriptorSet,
				.dstBinding = 0,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.pBufferInfo = &bufferInfo
			};
			vkUpdateDescriptorSets(*m_context->getDevice(), 1, &descriptorWrite, 0, nullptr);
		}
		
		// command pool and command buffer for buffer uploads
		VkCommandPoolCreateInfo commandPoolCreateInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
			.queueFamilyIndex = m_context->getDevice()->getPhysicalDevice()->getQueueFamilyIndices().graphicsFamily.value()
		};
		VkCommandPool bufferUploadCommandPool;
		vk::check(vkCreateCommandPool(*m_context->getDevice(), &commandPoolCreateInfo, nullptr, &bufferUploadCommandPool));

		VkCommandBufferAllocateInfo commandBufferAllocInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = bufferUploadCommandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1
		};
		VkCommandBuffer bufferUploadCommandBuffer;
		vk::check(vkAllocateCommandBuffers(*m_context->getDevice(), &commandBufferAllocInfo, &bufferUploadCommandBuffer));

		// model loading
		m_meshes = GltfLoader::loadFromFile("C:/Dev/C++/Vulkan/meshes/Suzanne.glb", m_context->getDevice(), bufferUploadCommandPool);

		// destroy pool for buffers
		vkDestroyCommandPool(*m_context->getDevice(), bufferUploadCommandPool, nullptr);
	}

	Renderer::Renderer(Renderer&& other) noexcept
		: m_context(std::move(other.m_context)),
		m_swapChain(std::move(other.m_swapChain)),
		m_currentFrameIndex(other.m_currentFrameIndex)
	{
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
		{
			m_frames[i] = std::move(other.m_frames[i]);

			other.m_frames[i] = FrameData{};
		}
	}

	Renderer& Renderer::operator=(Renderer&& other) noexcept 
	{
		if (this != &other) 
		{
			if (m_pipelineLayout != VK_NULL_HANDLE) 
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

			for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
			{
				m_frames[i] = std::move(other.m_frames[i]);
				other.m_frames[i] = FrameData{};
			}
		}
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
		frame.startTime = std::chrono::high_resolution_clock::now();

		// Wait for the render fence
		vk::check(vkWaitForFences(*m_context->getDevice(), 1, &frame.inFlightFence, VK_TRUE, UINT64_MAX));

		// update uniform buffer
		auto currentTime = std::chrono::high_resolution_clock::now();
		float timeSinceStart = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - m_appStartTime).count();

		m_camera.setAspectRatio(m_context->getWindow()->getWidth(), m_context->getWindow()->getHeight());
		m_camera.updateVectors3D();
		UniformData ubo{};
		ubo.model = glm::mat4(1.0f);
		ubo.view = m_camera.view();
		ubo.projection = m_camera.projection();
		ubo.time = timeSinceStart;

		// push updated data to uniform buffer
		frame.uniformBuffer->update(ubo);

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
		frame.endTime = std::chrono::high_resolution_clock::now();
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

		VkImageSubresourceRange colorSub{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		};

		// aspect mask with or without stencil bit depending on depth format
		VkImageAspectFlags depthAspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (m_swapChain->getDepthFormat() == VK_FORMAT_D32_SFLOAT_S8_UINT ||
			m_swapChain->getDepthFormat() == VK_FORMAT_D24_UNORM_S8_UINT)
		{
			depthAspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}

		VkImageSubresourceRange depthSub{
			.aspectMask = depthAspectMask,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		};

		// initial barriers for depth and color attachments
		VkImageMemoryBarrier2 initialBarriers[2] = {}; 
		
		// color attachment barrier
		initialBarriers[0] = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
			.srcAccessMask = VK_ACCESS_2_NONE,
			.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.image = m_swapChain->getImageHandles()[imageIndex],
			.subresourceRange = colorSub
		};

		// depth attachment barrier
		initialBarriers[1] = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
			.srcAccessMask = VK_ACCESS_2_NONE,
			.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
			.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
			.image = m_swapChain->getDepthImage(),
			.subresourceRange = depthSub
		};


		VkDependencyInfo dependencyInfo{
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.imageMemoryBarrierCount = 2,
			.pImageMemoryBarriers = initialBarriers	
		};

		vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);

		// color attachment info
		VkRenderingAttachmentInfo colorAttachmentInfo{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = m_swapChain->getImageViewHandles()[imageIndex],
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = VkClearValue{.color = {{0.01f, 0.01f, 0.03f, 1.0f}} },
		};

		// depth attachment info
		VkRenderingAttachmentInfo depthAttachmentInfo{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = m_swapChain->getDepthImageView(),
			.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.clearValue = VkClearValue{.depthStencil = {1.0f, 0} },
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
			.pDepthAttachment = &depthAttachmentInfo,
			.pStencilAttachment = nullptr
		};


		vkCmdBeginRendering(commandBuffer, &renderingInfo);

		// bind pipeline
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->get());

		// bind descriptor sets
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_frames[m_currentFrameIndex].descriptorSet, 0, nullptr);

		// meshes
		for (const auto& mesh : m_meshes)
		{
			mesh->bind(commandBuffer);
			vkCmdDrawIndexed(commandBuffer, mesh->getIndexCount(), 1, 0, 0, 0);
		}

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

	void Renderer::processInput(const InputManager& im)
	{
		m_camera.processInput(im);
	}

	bool Renderer::mouseWheelEvent(double x, double y)
	{
		m_camera.mouseWheelEvent(x, y);
		return true;
	}
}