#pragma once
#include "Vulkan/Core/Context.hpp"
#include "Vulkan/Core/CommandPool.hpp"
#include "Vulkan/Core/Semaphore.hpp"
#include "Vulkan/Core/Fence.hpp"
#include "Window/GlfwInstance.hpp"
#include "Window/Window.hpp"

#include <string>
#include <memory>

class Application
{
private:
	std::string m_name;
	GlfwInstance m_glfwInstance;
	std::shared_ptr<Window> m_window;
	std::unique_ptr<vk::core::Context> m_vkContext;
	static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
	uint32_t m_currentFrame = 0;
	std::unique_ptr<vk::core::CommandPool> m_commandPool;
	std::vector<VkCommandBuffer> m_commandBuffers;
	std::vector<vk::core::Semaphore> m_imageAvailableSemaphores;
	std::vector<vk::core::Semaphore> m_renderFinishedSemaphores;
	std::vector<vk::core::Fence> m_inFlightFences;
	
public:
	Application(std::string_view name, uint16_t width, uint16_t height);
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
	~Application();

	void run();

private:
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
};