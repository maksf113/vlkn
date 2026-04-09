#pragma once
#include "Vulkan/Core/Device.hpp"

#include <Vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace vk::core
{
    class CommandPool
    {
    private:
        VkCommandPool m_handle = VK_NULL_HANDLE;
		std::shared_ptr<Device> m_device;
    public:
        CommandPool(const std::shared_ptr<Device>& device);
        CommandPool(const CommandPool&) = delete;
        CommandPool& operator=(const CommandPool&) = delete;
        CommandPool(CommandPool&& other) noexcept;
        CommandPool& operator=(CommandPool&& other) noexcept;
        ~CommandPool();
        VkCommandPool get() const;
		operator VkCommandPool() const;
    };
}