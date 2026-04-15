#pragma once
#include "Vulkan/Core/Device.hpp"

#include <Vulkan/vulkan.h>

#include <memory>

namespace vk
{
    class RenderPass
    {
    private:
        VkRenderPass m_handle = VK_NULL_HANDLE;
		std::shared_ptr<Device> m_device;
    public:
        RenderPass(const std::shared_ptr<Device>& device, VkFormat format);
        RenderPass(const RenderPass&) = delete;
        RenderPass& operator=(const RenderPass&) = delete;
        RenderPass(RenderPass&& other) noexcept;
        RenderPass& operator=(RenderPass&& other) noexcept;
        ~RenderPass();
        VkRenderPass get() const;
		operator VkRenderPass() const;
    };
}