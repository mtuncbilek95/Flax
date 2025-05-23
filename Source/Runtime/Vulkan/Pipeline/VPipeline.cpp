#include "VPipeline.h"

#include <Runtime/Vulkan/Debug/VDebug.h>
#include <Runtime/Vulkan/Device/VDevice.h>
#include <Runtime/Vulkan/Utils/VUtils.h>
#include <Runtime/Vulkan/Descriptor/VDescriptorLayout.h>
#include <Runtime/Vulkan/Shader/VShader.h>
#include <Runtime/Vulkan/RenderPass/VRenderPass.h>

namespace Flax
{
	VPipeline::VPipeline(const GraphicsPipelineProps& desc, VDevice* pDevice) : VObject(pDevice), m_props(desc), m_bindPoint(VK_PIPELINE_BIND_POINT_GRAPHICS)
	{
		Vector<VkDescriptorSetLayout> dLayouts(desc.layouts.size());
		for (u32 i = 0; i < dLayouts.size(); i++)
			dLayouts[i] = desc.layouts[i]->GetVkLayout();

		Vector<VkPushConstantRange> ranges;

		for (u32 i = 0; i < desc.pushConstants.ranges.size(); i++)
		{
			VkPushConstantRange range = {};
			range.stageFlags = desc.pushConstants.ranges[i].flags;
			range.offset = desc.pushConstants.ranges[i].offset;
			range.size = desc.pushConstants.ranges[i].sizeOfCurrentPush;
		}

		VkPipelineLayoutCreateInfo layoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		layoutInfo.setLayoutCount = static_cast<u32>(dLayouts.size());
		layoutInfo.pSetLayouts = dLayouts.data();
		layoutInfo.pushConstantRangeCount = static_cast<u32>(ranges.size());
		layoutInfo.pPushConstantRanges = ranges.data();

		VDebug::VkAssert(vkCreatePipelineLayout(GetRoot()->GetVkDevice(), &layoutInfo, nullptr, &m_layout), "VPipeline");

		Vector<VkPipelineShaderStageCreateInfo> sStages;

		for (u32 i = 0; i < desc.shaderStages.size(); i++)
		{
			auto& shader = desc.shaderStages[i];
			VkPipelineShaderStageCreateInfo info = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
			info.module = shader->GetVkShaderModule();
			info.pName = shader->GetEntryPoint().c_str();
			info.stage = shader->GetVkShaderStage();
			sStages.push_back(info);
		}

		Vector<VkVertexInputBindingDescription> bindings;
		Vector<VkVertexInputAttributeDescription> attributes;
		for (u8 bindIndex = 0; bindIndex < desc.inputAssembler.bindings.size(); bindIndex++)
		{
			const auto& element = desc.inputAssembler.bindings[bindIndex];
			VkVertexInputBindingDescription binding = {};
			binding.binding = bindIndex;
			binding.inputRate = element.inputRate;

			u32 offset = 0;
			for (u8 attIndex = 0; attIndex < element.attributes.size(); attIndex++)
			{
				VkVertexInputAttributeDescription attr = {};
				attr.binding = bindIndex;
				attr.location = attIndex;
				attr.format = element.attributes[attIndex];
				attr.offset = offset;
				attributes.push_back(attr);

				offset += VUtils::GetVkTextureSize(element.attributes[attIndex]);
			}
			binding.stride = offset;
			bindings.push_back(binding);
		}

		// VertexInputState
		VkPipelineVertexInputStateCreateInfo vertexInputInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<u32>(bindings.size());
		vertexInputInfo.pVertexBindingDescriptions = bindings.data();
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<u32>(attributes.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributes.data();

		// InputAssemblyState
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		inputAssembly.topology = desc.inputAssembler.topology;
		inputAssembly.primitiveRestartEnable = desc.inputAssembler.primitiveRestartEnable;

		Vector<VkDynamicState> dStates;
		if (desc.viewportState.dynamicView)
			dStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
		if (desc.viewportState.dynamicScissor)
			dStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

		// DynamicPipelineState
		VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
		dynamicState.dynamicStateCount = static_cast<u32>(dStates.size());
		dynamicState.pDynamicStates = dStates.data();

		// ViewportState
		VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
		viewportState.viewportCount = 1;
		viewportState.pViewports = &desc.viewportState.viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &desc.viewportState.scissor;

		// RasterizationState
		VkPipelineRasterizationStateCreateInfo rasterizer = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = desc.rasterizerState.polygonMode;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = desc.rasterizerState.cullMode;
		rasterizer.frontFace = desc.rasterizerState.frontFace;
		rasterizer.depthBiasEnable = desc.rasterizerState.depthBiasEnable;
		rasterizer.depthBiasConstantFactor = desc.rasterizerState.depthBiasConstantFactor;
		rasterizer.depthBiasClamp = desc.rasterizerState.depthBiasClamp;
		rasterizer.depthBiasSlopeFactor = desc.rasterizerState.depthBiasSlopeFactor;

		// MultisampleState
		VkPipelineMultisampleStateCreateInfo multisampling = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		// ColorBlendAttachmentState
		Vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
		for (auto& attachment : desc.blendState.attachments)
		{
			VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
			colorBlendAttachment.colorWriteMask = attachment.colorWriteMask;

			colorBlendAttachment.blendEnable = attachment.blendEnable;

			colorBlendAttachment.srcColorBlendFactor = attachment.srcColorBlendFactor;
			colorBlendAttachment.dstColorBlendFactor = attachment.dstColorBlendFactor;
			colorBlendAttachment.colorBlendOp = attachment.colorBlendOp;

			colorBlendAttachment.srcAlphaBlendFactor = attachment.srcAlphaBlendFactor;
			colorBlendAttachment.dstAlphaBlendFactor = attachment.dstAlphaBlendFactor;
			colorBlendAttachment.alphaBlendOp = attachment.alphaBlendOp;

			colorBlendAttachments.push_back(colorBlendAttachment);
		}

		// ColorBlendState
		VkPipelineColorBlendStateCreateInfo colorBlending = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
		colorBlending.logicOpEnable = desc.blendState.logicOpEnable;
		colorBlending.logicOp = desc.blendState.logicOp;
		colorBlending.attachmentCount = static_cast<u32>(colorBlendAttachments.size());
		colorBlending.pAttachments = colorBlendAttachments.data();
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		// DepthStencilState
		VkPipelineDepthStencilStateCreateInfo depthstencil = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
		depthstencil.depthWriteEnable = desc.depthStencilState.depthWriteEnable;
		depthstencil.depthTestEnable = desc.depthStencilState.depthTestEnable;
		depthstencil.depthCompareOp = desc.depthStencilState.depthCompareOp;
		depthstencil.depthBoundsTestEnable = VK_FALSE;
		depthstencil.stencilTestEnable = VK_FALSE;
		depthstencil.minDepthBounds = 0.0f;
		depthstencil.maxDepthBounds = 1.0f;
		depthstencil.front = desc.depthStencilState.front;
		depthstencil.back = desc.depthStencilState.back;

		VkGraphicsPipelineCreateInfo pipelineInfo{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		pipelineInfo.stageCount = static_cast<u32>(sStages.size());
		pipelineInfo.pStages = sStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthstencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = dStates.size() > 0 ? &dynamicState : nullptr;
		pipelineInfo.layout = m_layout;
		pipelineInfo.renderPass = desc.renderPass->GetVkRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.flags = desc.createFlag;
		pipelineInfo.pNext = nullptr;

		VDebug::VkAssert(vkCreateGraphicsPipelines(pDevice->GetVkDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline), "VPipeline");
	}

	VPipeline::~VPipeline()
	{
		if (m_pipeline != VK_NULL_HANDLE)
		{
			vkDestroyPipeline(GetRoot()->GetVkDevice(), m_pipeline, nullptr);
			m_pipeline = VK_NULL_HANDLE;
		}

		if (m_layout != VK_NULL_HANDLE)
		{
			vkDestroyPipelineLayout(GetRoot()->GetVkDevice(), m_layout, nullptr);
			m_layout = VK_NULL_HANDLE;
		}
	}
}