#pragma once
#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <vector>
#include <string>

namespace vk::core
{
    class Instance
    {
    public:
        struct Config
        {
            std::string appName;
            std::vector<const char*> layers;
            std::vector<const char*> extensions;
            bool enableValidation;
        };
    private:
        VkInstance m_handle = VK_NULL_HANDLE;
        Config m_config;
    public:
        Instance(const Config& config);
        Instance(const Instance&) = delete;
        Instance& operator=(const Instance&) = delete;
        Instance(Instance&& other) noexcept;
        Instance& operator=(Instance&& other) noexcept;
        ~Instance();
        VkInstance get() const;
        operator VkInstance() const;
    private:
        void checkValidationLayerSupport();
		void checkExtensionSupport();
    };
}