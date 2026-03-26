#pragma once
#include "Vulkan/Core/Instance.hpp"

#include <Vulkan/vulkan.h>

#include <string>
#include <memory>

namespace vk::core
{
	class DebugMessenger
	{
	private:
		VkDebugUtilsMessengerEXT m_handle = VK_NULL_HANDLE;
		std::shared_ptr<Instance> m_instance;
	public:
		DebugMessenger(const std::shared_ptr<Instance>& instance);
		DebugMessenger(const DebugMessenger&) = delete;
		DebugMessenger& operator=(const DebugMessenger&) = delete;
		DebugMessenger(DebugMessenger&& other) noexcept;
		DebugMessenger& operator=(DebugMessenger&& other) noexcept;
		~DebugMessenger();
	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);
		static std::string severityToString(VkDebugUtilsMessageSeverityFlagBitsEXT severity);
		static std::string typeToString(VkDebugUtilsMessageTypeFlagsEXT type);
	};
}