#pragma once
#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/RenderPass.hpp"
#include "Vulkan/Core/SwapChain.hpp"
#include "Window/GlfwInstance.hpp"
#include "Window/Window.hpp"

#include <string>
#include <memory>
#include <chrono>

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

class Application
{
private:
	std::string m_name;
	GlfwInstance m_glfwInstance;
	std::shared_ptr<Window> m_window;

	std::unique_ptr<vk::Context> m_vkContext;
	std::shared_ptr<vk::SwapChain> m_swapChain;
	std::unique_ptr<vk::RenderPass> m_renderPass;

	static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
	FrameData m_frames[MAX_FRAMES_IN_FLIGHT];
	uint32_t m_currentFrameIndex = 0;
	
public:
	Application(std::string_view name, uint16_t width, uint16_t height);
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
	~Application();

	void run();

private:
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
};