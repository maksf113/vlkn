#pragma once
#include "Vulkan/Core/Instance.hpp"
#include "Vulkan/Core/DebugMessenger.hpp"
#include "Vulkan/Core/Surface.hpp"
#include "Vulkan/Core/PhysicalDevice.hpp"
#include "Vulkan/Core/Device.hpp"
#include "Window/Window.hpp"

#include <memory>
#include <string>
#include <vector>

namespace vk
{
	class Context
	{
	private:
		std::string m_name;
		Instance::Config m_instanceConfig;
		std::shared_ptr<Instance> m_vkInstance;
		std::unique_ptr<DebugMessenger> m_debugMessenger;
		std::shared_ptr<Surface> m_surface;
		std::shared_ptr<PhysicalDevice> m_physicalDevice;
		std::shared_ptr<Device> m_device;
		std::shared_ptr<Window> m_window;
		
	public:
		Context(std::string_view name, std::shared_ptr<Window>& window);
		Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;
		Context(Context&& other) noexcept = default;
		Context& operator=(Context&& other) noexcept = default;
		~Context() = default;

		const std::shared_ptr<Device>& getDevice() const;
		const std::shared_ptr<Surface>& getSurface() const;
		const std::shared_ptr<Window>& getWindow() const;
	};
}