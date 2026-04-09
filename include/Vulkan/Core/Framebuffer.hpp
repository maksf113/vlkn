#pragma once
#include "Vulkan/Core/Device.hpp"

#include <Vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace vk::core
{
    class Framebuffer
    {
        private:
         VkFramebuffer m_handle = VK_NULL_HANDLE;
         std::shared_ptr<Device> m_device;
     public:
         Framebuffer(const std::shared_ptr<Device>& device, VkRenderPass renderPass, const std::vector<VkImageView>& attachments, VkExtent2D extent);
         Framebuffer(const Framebuffer&) = delete;
         Framebuffer& operator=(const Framebuffer&) = delete;
         Framebuffer(Framebuffer&& other) noexcept;
         Framebuffer& operator=(Framebuffer&& other) noexcept;
         ~Framebuffer();
         VkFramebuffer get() const;
		 operator VkFramebuffer() const;
    };
}