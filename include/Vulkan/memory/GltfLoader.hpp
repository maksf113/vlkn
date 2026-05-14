#pragma once
#include "vulkan/core/Device.hpp"
#include "vulkan/memory/Mesh.hpp"

#include <vulkan/vulkan.h>
#include <tiny_gltf.h>

#include <memory>
#include <vector>
#include <string>

namespace vk
{
	class GltfLoader 
	{
	public:
		static std::vector<std::unique_ptr<Mesh>> loadFromFile(const std::string& filePath, const std::shared_ptr<Device>& device, VkCommandPool commandPool);
	};
}
