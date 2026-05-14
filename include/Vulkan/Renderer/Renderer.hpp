#pragma once
#include "vulkan/renderer/SwapChain.hpp"
#include "vulkan/renderer/Pipeline.hpp"
#include "vulkan/core/Context.hpp"
#include "vulkan/core/Device.hpp"
#include "vulkan/memory/VertexBuffer.hpp"
#include "vulkan/memory/IndexBuffer.hpp"
#include "vulkan/memory/UniformBuffer.hpp"
#include "vulkan/memory/Mesh.hpp"
#include "vulkan/memory/GltfLoader.hpp"
#include "vulkan/Camera.hpp"
#include "window/InputManager.hpp"
#include "window/InputReceiver.hpp"

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

		std::unique_ptr<UniformBuffer<UniformData>> uniformBuffer;
		VkDescriptorSet descriptorSet;

		std::chrono::high_resolution_clock::time_point startTime;
		std::chrono::high_resolution_clock::time_point endTime;
	};

	class Renderer : public InputReceiver
	{
	private:
		std::shared_ptr<vk::Context> m_context;
		std::unique_ptr<SwapChain> m_swapChain;
		VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
		std::unique_ptr<Pipeline> m_pipeline;
		std::vector<std::unique_ptr<Mesh>> m_meshes;

		static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
		FrameData m_frames[MAX_FRAMES_IN_FLIGHT];
		uint32_t m_currentFrameIndex = 0;

		std::chrono::high_resolution_clock::time_point m_appStartTime;

		Camera m_camera;

	public:
		Renderer(const std::shared_ptr<Context>& context);
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer(Renderer&& other) noexcept;
		Renderer& operator=(Renderer&& other) noexcept;
		~Renderer();

		void drawFrame();
		void recordDynamicCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);	
		void recreateSwapChain();
		void processInput(const InputManager& im);
		bool mouseWheelEvent(double x, double y) override;
	};
}
	