#include "Vulkan/Core/DebugMessenger.hpp"
#include "Vulkan/Utility.hpp"

#include <iostream>

namespace vk
{
	DebugMessenger::DebugMessenger(const std::shared_ptr<Instance>& instance) : m_instance(instance)
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.pNext = nullptr,
			.flags = 0,
			.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
							   VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
							   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
							   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
						   VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
						   VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			.pfnUserCallback = debugCallback,
			.pUserData = nullptr
		};
		auto createFunc = loadFunction<PFN_vkCreateDebugUtilsMessengerEXT>(*m_instance.get(), "vkCreateDebugUtilsMessengerEXT");
		check(createFunc(*m_instance.get(), &createInfo, nullptr, &m_handle));
	}

	DebugMessenger::DebugMessenger(DebugMessenger&& other) noexcept :
		m_handle(other.m_handle), m_instance(std::move(other.m_instance))
	{
		other.m_handle = VK_NULL_HANDLE;
	}

	DebugMessenger& DebugMessenger::operator=(DebugMessenger&& other) noexcept
	{
		if(m_handle != VK_NULL_HANDLE)
		{
			auto destroyFunc = loadFunction<PFN_vkDestroyDebugUtilsMessengerEXT>(*m_instance.get(), "vkDestroyDebugUtilsMessengerEXT");
			destroyFunc(*m_instance.get(), m_handle, nullptr);
		}
		m_handle = other.m_handle;
		m_instance = std::move(other.m_instance);
		other.m_handle = VK_NULL_HANDLE;
		return *this;
	}

	DebugMessenger::~DebugMessenger()
	{
		if (m_handle != VK_NULL_HANDLE)
		{
			auto destroyFunc = loadFunction<PFN_vkDestroyDebugUtilsMessengerEXT>(*m_instance.get(), "vkDestroyDebugUtilsMessengerEXT");
			destroyFunc(*m_instance.get(), m_handle, nullptr);
		}
	}
	VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessenger::debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		std::cerr << "[" << severityToString(messageSeverity) << "] "
			<< "[" << typeToString(messageType) << "] "
			<< pCallbackData->pMessage << std::endl
			<< "Objects:" << std::endl;
		for (uint32_t i = 0; i < pCallbackData->objectCount; ++i)
		{
			const VkDebugUtilsObjectNameInfoEXT& objectInfo = pCallbackData->pObjects[i];
			std::cerr << "\tObject " << i << ": "
				<< "Type=" << objectInfo.objectType << ", "
				<< "Handle=" << objectInfo.objectHandle << ", "
				<< "Name=" << (objectInfo.pObjectName ? objectInfo.pObjectName : "Unnamed")
				<< std::endl;
		}
		return VK_FALSE;
	}

	std::string DebugMessenger::severityToString(VkDebugUtilsMessageSeverityFlagBitsEXT severity)
	{
		switch (severity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			return "VERBOSE";
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			return "INFO";
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			return "WARNING";
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			return "ERROR";
		default:
			return "UNKNOWN";
		}
	}

	std::string DebugMessenger::typeToString(VkDebugUtilsMessageTypeFlagsEXT type)
	{
		switch (type)
		{
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
			return "GENERAL";
		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
			return "VALIDATION";
		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
			return "PERFORMANCE";
		case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
			return "DEVICE_ADDRESS_BINDING";
		default:
			return "UNKNOWN";
		}
	}

}