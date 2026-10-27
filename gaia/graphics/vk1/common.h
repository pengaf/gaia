#pragma once
#include "utility.h"
#include "../common.h"
#include "detail/vulkan.h"
#include <vector>

BEGIN_GAIA_VK1

struct DrawCommand
{
	uint32_t m_vertexCount = 0;
	uint32_t m_instanceCount = 0;
	uint32_t m_firstVertex = 0;
	uint32_t m_firstInstance = 0;
public:
	void initialize(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0)
	{
		m_vertexCount = vertexCount;
		m_instanceCount = instanceCount;
		m_firstVertex = firstVertex;
		m_firstInstance = firstInstance;
	}
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
	uint32_t m_groupCountX = 0;
	uint32_t m_groupCountY = 0;
	uint32_t m_groupCountZ = 0;
public:
	void initialize(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
	{
		m_groupCountX = groupCountX;
		m_groupCountY = groupCountY;
		m_groupCountZ = groupCountZ;
	}
	void execute(VkCommandBuffer commandBuffer)
	{
		vkCmdDispatch(commandBuffer, m_groupCountX, m_groupCountY, m_groupCountZ);
	}
};

class TraceRaysCommand
{
	VkStridedDeviceAddressRegionKHR m_raygenShaderBindingTable{};
	VkStridedDeviceAddressRegionKHR m_missShaderBindingTable{};
	VkStridedDeviceAddressRegionKHR m_hitShaderBindingTable{};
	VkStridedDeviceAddressRegionKHR m_callableShaderBindingTable{};
	uint32_t m_width = 0;
	uint32_t m_height = 0;
	uint32_t m_depth = 0;
public:
	void execute(VkCommandBuffer commandBuffer) const
	{
		vkCmdTraceRaysKHR(commandBuffer, &m_raygenShaderBindingTable, &m_missShaderBindingTable, &m_hitShaderBindingTable, &m_callableShaderBindingTable, m_width, m_height, m_depth);
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

template<VkPipelineBindPoint t_bindPoint>
struct BindPipelineCommand
{
	VkPipeline m_pipeline;
	void execute(VkCommandBuffer commandBuffer)
	{
		vkCmdBindPipeline(commandBuffer, t_bindPoint, m_pipeline);
	}
};

template<VkPipelineBindPoint t_bindPoint>
struct BindDescriptorSetsCommand
{
	VkPipelineLayout m_pipelineLayout;
	VkDescriptorSet m_descriptorSets[gaia_max_bind_group_count];
	uint32_t m_descriptorSetCount;
	void execute(VkCommandBuffer commandBuffer)
	{
		if (m_descriptorSetCount)
		{
			vkCmdBindDescriptorSets(
				commandBuffer,
				t_bindPoint,
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
	VkRenderingAttachmentInfo m_depthStencilAttachmentInfo;
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
		m_renderingInfo.pDepthAttachment = &m_depthStencilAttachmentInfo;
		m_renderingInfo.pStencilAttachment = &m_depthStencilAttachmentInfo;
		for (uint32_t i = 0; i < gaia_max_color_attment_count; ++i)
		{
			m_colorAttachmentInfos[i].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			m_colorAttachmentInfos[i].pNext = nullptr;
			m_colorAttachmentInfos[i].imageView = VK_NULL_HANDLE;
			m_colorAttachmentInfos[i].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			m_colorAttachmentInfos[i].resolveMode = VK_RESOLVE_MODE_NONE;
			m_colorAttachmentInfos[i].resolveImageView = VK_NULL_HANDLE;
			m_colorAttachmentInfos[i].resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			m_colorAttachmentInfos[i].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			m_colorAttachmentInfos[i].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}
		m_depthStencilAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		m_depthStencilAttachmentInfo.pNext = nullptr;
		m_depthStencilAttachmentInfo.imageView = VK_NULL_HANDLE;
		m_depthStencilAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		m_depthStencilAttachmentInfo.resolveMode = VK_RESOLVE_MODE_NONE;
		m_depthStencilAttachmentInfo.resolveImageView = VK_NULL_HANDLE;
		m_depthStencilAttachmentInfo.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		m_depthStencilAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		m_depthStencilAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	}
	void execute(VkCommandBuffer commandBuffer)
	{
		vkCmdBeginRendering(commandBuffer, &m_renderingInfo);
	}
};


END_GAIA_VK1