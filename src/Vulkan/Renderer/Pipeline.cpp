#include "Vulkan/Renderer/Pipeline.hpp"
#include "Vulkan/Utility.hpp"

#include <fstream>
#include <iostream>

namespace vk
{
	Pipeline::Pipeline(const std::shared_ptr<Context>& context, const PipelineConfig& config) :
		m_device(context->getDevice()), m_pipelineLayoutHandle(config.pipelineLayout)
	{
		// shaders
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		std::vector<VkShaderModule> shaderModules;

		for(const auto& shaderConfig : config.shaderConfigs)
		{
			std::vector<char> shaderCode = ::readFile(shaderConfig.FilePath);
			VkShaderModule shaderModule = createShaderModule(shaderCode);
			shaderModules.push_back(shaderModule);
			VkPipelineShaderStageCreateInfo shaderStageCreateInfo{};
			shaderStageCreateInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageCreateInfo.stage  = shaderConfig.stage;
			shaderStageCreateInfo.module = shaderModule;
			shaderStageCreateInfo.pName  = "main";

			shaderStages.push_back(shaderStageCreateInfo);
		}


		VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineCreateInfo.pStages = shaderStages.data();
		pipelineCreateInfo.pVertexInputState = &config.vertexInputStateCreateInfo;
		pipelineCreateInfo.pInputAssemblyState = &config.inputAssemblyStateCreateInfo;
		pipelineCreateInfo.pViewportState = &config.viewportStateCreateInfo;
		pipelineCreateInfo.pRasterizationState = &config.rasterizationStateCreateInfo;
		pipelineCreateInfo.pMultisampleState = &config.multisampleStateCreateInfo;
		pipelineCreateInfo.pDepthStencilState = &config.depthStencilStateCreateInfo;
		pipelineCreateInfo.pColorBlendState = &config.colorBlendStateCreateInfo;
		pipelineCreateInfo.pDynamicState = &config.dynamicStateCreateInfo;
		pipelineCreateInfo.layout = config.pipelineLayout;
		pipelineCreateInfo.renderPass = config.renderPass;
		pipelineCreateInfo.subpass = config.subpasss;

		if (vkCreateGraphicsPipelines(m_device->get(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_handle) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create graphics pipeline");
		}

		for(VkShaderModule shaderModule : shaderModules)
		{
			vkDestroyShaderModule(m_device->get(), shaderModule, nullptr);
		}
	}
	Pipeline::~Pipeline()
	{
		if (m_handle != VK_NULL_HANDLE)
		{
			vkDestroyPipeline(m_device->get(), m_handle, nullptr);
		}
	}

	VkPipeline Pipeline::get() const
	{
		return m_handle;
	}

	Pipeline::operator VkPipeline() const
	{
		return m_handle;
	}

	VkShaderModule Pipeline::createShaderModule(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode    = reinterpret_cast<const uint32_t*>(code.data());
		VkShaderModule shaderModule;
		if (vkCreateShaderModule(m_device->get(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module");
		}
		return shaderModule;
	}

	void Pipeline::defaultPipelineConfig(PipelineConfig& config)
	{
		// vertex input
		config.vertexInputStateCreateInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		config.vertexInputStateCreateInfo.vertexBindingDescriptionCount   = 0;
		config.vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
		config.vertexInputStateCreateInfo.pVertexBindingDescriptions      = nullptr;
		config.vertexInputStateCreateInfo.pVertexAttributeDescriptions    = nullptr;

		// input assembly state
		config.inputAssemblyStateCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		config.inputAssemblyStateCreateInfo.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;	
		config.inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		// viewport, scissors
		config.viewportStateCreateInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;	
		config.viewportStateCreateInfo.viewportCount = 1;
		config.viewportStateCreateInfo.scissorCount  = 1;
		config.viewportStateCreateInfo.pViewports    = nullptr; // dynamic ?
		config.viewportStateCreateInfo.pScissors     = nullptr; // dynamic

		// rasterizer
		config.rasterizationStateCreateInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		config.rasterizationStateCreateInfo.depthClampEnable        = VK_FALSE;	
		config.rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;	
		config.rasterizationStateCreateInfo.polygonMode             = VK_POLYGON_MODE_FILL;	
		config.rasterizationStateCreateInfo.lineWidth               = 1.0f;
		config.rasterizationStateCreateInfo.cullMode                = VK_CULL_MODE_NONE;	
		config.rasterizationStateCreateInfo.frontFace               = VK_FRONT_FACE_CLOCKWISE;	
		config.rasterizationStateCreateInfo.depthBiasEnable         = VK_FALSE;

		// multisampling
		config.multisampleStateCreateInfo.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;	
		config.multisampleStateCreateInfo.sampleShadingEnable  = VK_FALSE;	
		config.multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;	

		// color blend attachment
		config.colorBlendAttachmentState.blendEnable    = VK_FALSE;
		config.colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		// color blending
		config.colorBlendStateCreateInfo.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;	
		config.colorBlendStateCreateInfo.logicOpEnable   = VK_FALSE;	
		config.colorBlendStateCreateInfo.attachmentCount = 1;
		config.colorBlendStateCreateInfo.pAttachments	 = &config.colorBlendAttachmentState;
		
		// depth stencil
		config.depthStencilStateCreateInfo.sType            = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		config.depthStencilStateCreateInfo.depthTestEnable  = VK_TRUE;
		config.depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
		config.depthStencilStateCreateInfo.depthCompareOp   = VK_COMPARE_OP_LESS;

		// dynamic state
		config.dynamicStateEnables                      = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		config.dynamicStateCreateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		config.dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(config.dynamicStateEnables.size());
		config.dynamicStateCreateInfo.pDynamicStates    = config.dynamicStateEnables.data();	
	}
}