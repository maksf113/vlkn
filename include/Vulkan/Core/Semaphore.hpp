#pragma once
#include "Vulkan/Core/Device.hpp"

#include <Vulkan/vulkan.h>

#include <memory>

namespace vk::core
{
    class Semaphore
    {
    private:
        VkSemaphore m_handle = VK_NULL_HANDLE;
		std::shared_ptr<Device> m_device;
    public:
        Semaphore(const std::shared_ptr<Device>& device);
        Semaphore(const Semaphore&) = delete;
        Semaphore& operator=(const Semaphore&) = delete;
        Semaphore(Semaphore&& other) noexcept;
        Semaphore& operator=(Semaphore&& other) noexcept;
        ~Semaphore();
        VkSemaphore get() const;
		operator VkSemaphore() const;
    };
}