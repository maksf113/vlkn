#pragma once
#include "Vulkan/Core/Device.hpp"

#include <Vulkan/vulkan.h>

#include <memory>

namespace vk::core
{
    class Fence
    {
    private:
        VkFence m_handle = VK_NULL_HANDLE;
		std::shared_ptr<Device> m_device;
    public:
        Fence(const std::shared_ptr<Device>& device, bool signaled = false);
        Fence(const Fence&) = delete;
        Fence& operator=(const Fence&) = delete;
        Fence(Fence&& other) noexcept;
        Fence& operator=(Fence&& other) noexcept;
        ~Fence();
        VkFence get() const;
		operator VkFence() const;
		void wait(uint64_t timeout = UINT64_MAX) const;
		void reset() const; 
    };
}