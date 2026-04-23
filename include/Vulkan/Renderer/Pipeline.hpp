#pragma once
#include "Vulkan/Core/Context.hpp"

#include <Vulkan/vulkan.h>

namespace vk
{
	struct ShaderStageConfig
	{
		VkShaderStageFlagBits stage;
		std::string FilePath;
	};
	struct PipelineConfig
	{
		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
		VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
		VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};	
		VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};

		// dynamic memory
		std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;	
		std::vector<VkDynamicState>	dynamicStateEnables;

		VkPipelineLayout pipelineLayout;
		VkRenderPass renderPass;
		uint32_t subpasss = 0;

		std::vector<ShaderStageConfig> shaderConfigs;

		void addShaderStage(VkShaderStageFlagBits stage, const std::string& filePath)
		{
			shaderConfigs.push_back({ stage, filePath });
		}

		void addDynamicState(VkDynamicState state)
		{
			dynamicStateEnables.push_back(state);
		}
	};

	class Pipeline
	{
	private:
		VkPipeline m_handle = VK_NULL_HANDLE;
		VkPipelineLayout m_pipelineLayoutHandle = VK_NULL_HANDLE;

		std::shared_ptr<Device> m_device;

	public:
		Pipeline(const std::shared_ptr<Context>& context, const PipelineConfig& config);
		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;
		Pipeline(Pipeline&& other) noexcept;
		Pipeline& operator=(Pipeline&& other) noexcept;
		~Pipeline();

		static void defaultPipelineConfig(PipelineConfig& config);

		VkPipeline get() const;
		operator VkPipeline() const;

	private:
		VkShaderModule createShaderModule(const std::vector<char>& code);
	};
}