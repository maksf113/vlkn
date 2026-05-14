#pragma once
#include "vulkan/memory/Utility.hpp"
#include "vulkan/core/Device.hpp"
#include "vulkan/Utility.hpp"

#include <Vulkan/vulkan.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace vk 
{
    struct UniformData
    {
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 model;
        float time;
	};


    template <typename T>
    class UniformBuffer 
    {
    private:
        VkBuffer m_handle = VK_NULL_HANDLE;
        VkDeviceMemory m_bufferMemory = VK_NULL_HANDLE;
        std::shared_ptr<Device> m_device;
        void* m_mappedData = nullptr;

    public:
        UniformBuffer(const std::shared_ptr<Device>& device);
        UniformBuffer(const UniformBuffer&) = delete;
        UniformBuffer& operator=(const UniformBuffer&) = delete;
        UniformBuffer(UniformBuffer&& other) noexcept;
        UniformBuffer& operator=(UniformBuffer&& other) noexcept;
        ~UniformBuffer();

        VkBuffer get() const;
        operator VkBuffer() const;

        void update(const T& data);
    };

    template<typename T>
    UniformBuffer<T>::UniformBuffer(const std::shared_ptr<Device>& device) : m_device(device) 
    {
        VkDeviceSize bufferSize = sizeof(T);

        createBuffer(m_device, bufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            static_cast<VkMemoryPropertyFlagBits>(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
            &m_handle, &m_bufferMemory);

        check(vkMapMemory(*m_device, m_bufferMemory, 0, bufferSize, 0, &m_mappedData));
    }

    template<typename T>
    UniformBuffer<T>::UniformBuffer(UniformBuffer&& other) noexcept :
        m_handle(other.m_handle),
        m_bufferMemory(other.m_bufferMemory),
        m_device(std::move(other.m_device)),
        m_mappedData(other.m_mappedData) 
    {
        other.m_handle = VK_NULL_HANDLE;
        other.m_bufferMemory = VK_NULL_HANDLE;
        other.m_mappedData = nullptr;
    }

    template<typename T>
    UniformBuffer<T>& UniformBuffer<T>::operator=(UniformBuffer&& other) noexcept 
    {
        if (this != &other) {
            if (m_handle != VK_NULL_HANDLE) {
                vkDestroyBuffer(*m_device, m_handle, nullptr);
            }
            if (m_bufferMemory != VK_NULL_HANDLE) {
                vkUnmapMemory(*m_device, m_bufferMemory);
                vkFreeMemory(*m_device, m_bufferMemory, nullptr);
            }

            m_handle = other.m_handle;
            m_bufferMemory = other.m_bufferMemory;
            m_device = std::move(other.m_device);
            m_mappedData = other.m_mappedData;

            other.m_handle = VK_NULL_HANDLE;
            other.m_bufferMemory = VK_NULL_HANDLE;
            other.m_mappedData = nullptr;
        }
        return *this;
    }

    template<typename T>
    UniformBuffer<T>::~UniformBuffer() 
    {
        if (m_handle != VK_NULL_HANDLE) 
        {
            vkDestroyBuffer(*m_device, m_handle, nullptr);
        }
        if (m_bufferMemory != VK_NULL_HANDLE) 
        {
            vkUnmapMemory(*m_device, m_bufferMemory);
            vkFreeMemory(*m_device, m_bufferMemory, nullptr);
        }
    }

    template<typename T>
    VkBuffer UniformBuffer<T>::get() const 
    {
        return m_handle;
    }

    template<typename T>
    UniformBuffer<T>::operator VkBuffer() const 
    {
        return m_handle;
    }

    template<typename T>
    void UniformBuffer<T>::update(const T& data) 
    {
        std::memcpy(m_mappedData, &data, sizeof(T));
    }

} 