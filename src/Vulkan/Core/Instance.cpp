#include "vulkan/core/Instance.hpp"
#include "vulkan/Utility.hpp"

#include <stdexcept>
#include <utility>

namespace vk
{
    Instance::Instance(const Config& config) : m_config(config)
    {
        VkApplicationInfo appInfo{
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = config.appName.c_str(),
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "No Engine",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_3
        };

        VkInstanceCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = static_cast<uint32_t>(config.layers.size()),
            .ppEnabledLayerNames = config.layers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(config.extensions.size()),
            .ppEnabledExtensionNames = config.extensions.data()  
        };
        if(config.enableValidation)
        {
            checkValidationLayerSupport();
		}
        checkExtensionSupport();
        check(vkCreateInstance(&createInfo, nullptr, &m_handle));
    }
    Instance::Instance(Instance&& other) noexcept :
        m_handle(other.m_handle), m_config(std::move(other.m_config))
    {
        other.m_handle = VK_NULL_HANDLE;
    }
    Instance& Instance::operator=(Instance&& other) noexcept
    {
        if(m_handle != VK_NULL_HANDLE)
        {
            vkDestroyInstance(m_handle, nullptr);
        }
        m_handle = other.m_handle;
        m_config = std::move(other.m_config);
        other.m_handle = VK_NULL_HANDLE;
		return *this;
    }
    Instance::~Instance()
    {
        if (m_handle != VK_NULL_HANDLE)
        {
            vkDestroyInstance(m_handle, nullptr);
        }
    }
    VkInstance Instance::get() const
    {
        return m_handle;
    }
    Instance::operator VkInstance() const
    {
        return m_handle;
    }

    void Instance::checkValidationLayerSupport()
    {
        uint32_t layerCount;
        check(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
        std::vector<VkLayerProperties> availableLayers(layerCount);
        check(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()));

        for (const char* layerName : m_config.layers)
        {
            bool layerFound = false;
            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }   
            }
            if (!layerFound)
            {
                throw std::runtime_error("Validation layer not found: " + std::string(layerName));
			}
        }
    }

    void Instance::checkExtensionSupport()
    {
        uint32_t extensionCount;
        check(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        check(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data()));
        for (const char* extensionName : m_config.extensions)
        {
            bool extensionFound = false;
            for (const auto& extensionProperties : availableExtensions)
            {
                if (strcmp(extensionName, extensionProperties.extensionName) == 0)
                {
                    extensionFound = true;
                    break;
                }
            }
            if (!extensionFound)
            {
                throw std::runtime_error("Extension not found: " + std::string(extensionName));
            }
        }
    }

}