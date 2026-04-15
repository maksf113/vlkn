#pragma once
#include "Vulkan/Core/Instance.hpp"

#include <Vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <source_location>

namespace vk
{

	template <typename T>
	inline T loadFunction(Instance& instance, const char* funcName)
	{
		auto funcPtr = reinterpret_cast<T>(vkGetInstanceProcAddr(instance, funcName));
		if (!funcPtr)
		{
			throw std::runtime_error(std::string("Failed to load function: ") + funcName);
		}
		return funcPtr;
	}
	inline const char* resultToString(VkResult err)
	{
		switch (err) {
		case VK_SUCCESS:                     return "VK_SUCCESS";
		case VK_NOT_READY:                   return "VK_NOT_READY";
		case VK_TIMEOUT:                     return "VK_TIMEOUT";
		case VK_EVENT_SET:                   return "VK_EVENT_SET";
		case VK_EVENT_RESET:                 return "VK_EVENT_RESET";
		case VK_INCOMPLETE:                  return "VK_INCOMPLETE";
		case VK_ERROR_OUT_OF_HOST_MEMORY:    return "VK_ERROR_OUT_OF_HOST_MEMORY";
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:  return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
		case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
		case VK_ERROR_DEVICE_LOST:           return "VK_ERROR_DEVICE_LOST";
		case VK_ERROR_MEMORY_MAP_FAILED:     return "VK_ERROR_MEMORY_MAP_FAILED";
		case VK_ERROR_LAYER_NOT_PRESENT:     return "VK_ERROR_LAYER_NOT_PRESENT";
		case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
		case VK_ERROR_FEATURE_NOT_PRESENT:   return "VK_ERROR_FEATURE_NOT_PRESENT";
		case VK_ERROR_INCOMPATIBLE_DRIVER:   return "VK_ERROR_INCOMPATIBLE_DRIVER";
		case VK_ERROR_TOO_MANY_OBJECTS:      return "VK_ERROR_TOO_MANY_OBJECTS";
		case VK_ERROR_FORMAT_NOT_SUPPORTED:  return "VK_ERROR_FORMAT_NOT_SUPPORTED";
		case VK_ERROR_SURFACE_LOST_KHR:      return "VK_ERROR_SURFACE_LOST_KHR";
		case VK_SUBOPTIMAL_KHR:              return "VK_SUBOPTIMAL_KHR";
		case VK_ERROR_OUT_OF_DATE_KHR:       return "VK_ERROR_OUT_OF_DATE_KHR";
		default:                             return "VK_ERROR_UNKNOWN_OR_UNHANDLED";
		}
	}
	inline void check(VkResult result, const std::source_location& location = std::source_location::current())
	{
		if (result < 0) // errors are negative values
		{
			throw std::runtime_error(
				std::string("Vulkan error: ") +
				resultToString(result) + " at " +
				location.file_name() + ":" +
				std::to_string(location.line())
			);
		}
#ifdef DEBUG_MODE
		if (result > 0) // warnings are positive values
		{
			std::clog << "Vulkan warning: " << 
				resultToString(result) << " at " <<
			location.file_name() << ":" <<
			std::to_string(location.line()) << std::endl;
		}
#endif
	}
}

