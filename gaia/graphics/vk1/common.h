#pragma once
#include "utility.h"
#include "../common.h"
#include "detail/vulkan.h"
#include <vector>

BEGIN_GAIA_VK1

struct DrawCommand
{
	uint32_t m_vertexCount;
	uint32_t m_instanceCount;
	uint32_t m_firstVertex;
	uint32_t m_firstInstance;
	void execute(VkCommandBuffer commandBuffer)
	{
		vkCmdDraw(commandBuffer, m_vertexCount, m_instanceCount, m_firstVertex, m_firstInstance);
	}
};

struct DrawIndexedCommand
{
	uint32_t m_indexCount;
	uint32_t m_instanceCount;
	uint32_t m_firstIndex;
	int32_t m_vertexOffset;
	uint32_t m_firstInstance;
	void execute(VkCommandBuffer commandBuffer)
	{
		vkCmdDrawIndexed(commandBuffer, m_indexCount, m_instanceCount, m_firstIndex, m_vertexOffset, m_firstInstance);
	}
};

struct DispatchCommand
{
	uint32_t m_groupCountX;
	uint32_t m_groupCountY;
	uint32_t m_groupCountZ;
	void execute(VkCommandBuffer commandBuffer)
	{
		vkCmdDispatch(commandBuffer, m_groupCountX, m_groupCountY, m_groupCountZ);
	}
};

struct BarrierCommandSet
{
	struct BarrierCommand
	{
		VkPipelineStageFlags m_srcStageMask;
		VkPipelineStageFlags m_dstStageMask;
		VkDependencyFlags m_dependencyFlags;
		uint32_t m_startBufferMemoryBarrier;
		uint32_t m_bufferMemoryBarrierCount;
		uint32_t m_startImageMemoryBarrier;
		uint32_t m_imageMemoryBarrierCount;
	};
	std::vector<VkBufferMemoryBarrier> m_bufferBarriers;
	std::vector<VkImageMemoryBarrier> m_imageBarriers;
	std::vector<BarrierCommand> m_barrierCommands;
	void execute(VkCommandBuffer commandBuffer)
	{
		for (auto& pbc : m_barrierCommands)
		{
			GAIA_ASSERT(0 == pbc.m_bufferMemoryBarrierCount || pbc.m_bufferMemoryBarrierCount + pbc.m_startBufferMemoryBarrier <= m_bufferBarriers.size());
			GAIA_ASSERT(0 == pbc.m_imageMemoryBarrierCount || pbc.m_imageMemoryBarrierCount + pbc.m_startImageMemoryBarrier <= m_imageBarriers.size());
			vkCmdPipelineBarrier(
				commandBuffer,
				pbc.m_srcStageMask,
				pbc.m_dstStageMask,
				pbc.m_dependencyFlags,
				0,
				nullptr,
				pbc.m_bufferMemoryBarrierCount,
				pbc.m_bufferMemoryBarrierCount ? m_bufferBarriers.data() + pbc.m_startBufferMemoryBarrier : nullptr,
				pbc.m_imageMemoryBarrierCount,
				pbc.m_imageMemoryBarrierCount ? m_imageBarriers.data() + pbc.m_startImageMemoryBarrier : nullptr);
		}
	}
};

struct BindPipelineCommand
{
	VkPipeline m_pipeline;
	VkPipelineBindPoint m_bindPoint;

	void execute(VkCommandBuffer commandBuffer)
	{
		vkCmdBindPipeline(commandBuffer, m_bindPoint, m_pipeline);
	}
};

struct BindDescriptorSetsCommand
{
	VkPipelineLayout m_pipelineLayout;
	VkDescriptorSet m_descriptorSets[gaia_max_bind_group_count];
	uint32_t m_descriptorSetCount;
	VkPipelineBindPoint m_bindPoint;
	void execute(VkCommandBuffer commandBuffer)
	{
		if (m_descriptorSetCount)
		{
			vkCmdBindDescriptorSets(
				commandBuffer,
				m_bindPoint,
				m_pipelineLayout,
				0,
				m_descriptorSetCount,
				m_descriptorSets,
				0,
				nullptr);
		}
	}
};

struct BeginRenderingCommand
{
	VkRenderingInfo m_renderingInfo;
	VkRenderingAttachmentInfo m_colorAttachmentInfos[gaia_max_color_attment_count];
	VkRenderingAttachmentInfo m_depthAttachment;
	VkRenderingAttachmentInfo m_stencilAttachment;
public:
	BeginRenderingCommand()
	{
		m_renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		m_renderingInfo.pNext = nullptr;
		m_renderingInfo.flags = 0;
		m_renderingInfo.renderArea.offset = { 0, 0 };
		m_renderingInfo.renderArea.extent = { 0, 0 };
		m_renderingInfo.layerCount = 1;
		m_renderingInfo.viewMask = 0;
		m_renderingInfo.colorAttachmentCount = 0;
		m_renderingInfo.pColorAttachments = m_colorAttachmentInfos;
		m_renderingInfo.pDepthAttachment = &m_depthAttachment;
		m_renderingInfo.pStencilAttachment = &m_stencilAttachment;
		for (uint32_t i = 0; i < gaia_max_color_attment_count; ++i)
		{
			m_colorAttachmentInfos[i].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			m_colorAttachmentInfos[i].pNext = nullptr;
			m_colorAttachmentInfos[i].imageView = VK_NULL_HANDLE;
			m_colorAttachmentInfos[i].imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			m_colorAttachmentInfos[i].resolveMode = VK_RESOLVE_MODE_NONE;
			m_colorAttachmentInfos[i].resolveImageView = VK_NULL_HANDLE;
			m_colorAttachmentInfos[i].resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			m_colorAttachmentInfos[i].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			m_colorAttachmentInfos[i].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}
		m_depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		m_depthAttachment.pNext = nullptr;
		m_depthAttachment.imageView = VK_NULL_HANDLE;
		m_depthAttachment.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		m_depthAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
		m_depthAttachment.resolveImageView = VK_NULL_HANDLE;
		m_depthAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		m_depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		m_depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		m_stencilAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		m_stencilAttachment.pNext = nullptr;
		m_stencilAttachment.imageView = VK_NULL_HANDLE;
		m_stencilAttachment.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		m_stencilAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
		m_stencilAttachment.resolveImageView = VK_NULL_HANDLE;
		m_stencilAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		m_stencilAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		m_stencilAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	}
	void execute(VkCommandBuffer commandBuffer)
	{
		vkCmdBeginRendering(commandBuffer, &m_renderingInfo);
	}
};


END_GAIA_VK1