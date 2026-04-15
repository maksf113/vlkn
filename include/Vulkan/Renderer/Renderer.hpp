#pragma once
#include "Vulkan/Renderer/SwapChain.hpp"
#include "Vulkan/Renderer/RenderPass.hpp"
#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/Device.hpp"


#include <Vulkan/vulkan.h>

#include <chrono>
#include <memory>

namespace vk
{
	struct FrameData
	{
		VkCommandPool commandPool;
		VkCommandBuffer commandBuffer;
		VkSemaphore imageAvailableSemaphore;
		VkSemaphore renderFinishedSemaphore;
		VkFence inFlightFence;

		std::chrono::high_resolution_clock::time_point startTime;
		std::chrono::high_resolution_clock::time_point endTime;
	};

	class Renderer
	{
	private:
		std::shared_ptr<vk::Context> m_context;
		std::unique_ptr<SwapChain> m_swapChain;
		std::unique_ptr<RenderPass> m_renderPass;

		static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
		FrameData m_frames[MAX_FRAMES_IN_FLIGHT];
		uint32_t m_currentFrameIndex = 0;

	public:
		Renderer(const std::shared_ptr<Context>& context);
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer(Renderer&& other) noexcept;
		Renderer& operator=(Renderer&& other) noexcept;
		~Renderer();

		void drawFrame();
		void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	};
}
	