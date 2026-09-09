#pragma once
#include "utility.h"
#include "../common.h"
#include "detail/vulkan.h"
#include <vector>

BEGIN_GAIA_VK1

struct DrawCommand
{
	uint32_t vertexCount;
	uint32_t instanceCount;
	uint32_t firstVertex;
	uint32_t firstInstance;
	void execute(VkCommandBuffer commandBuffer)
	{
		vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
	}
};

struct DrawIndexedCommand
{
	uint32_t indexCount;
	uint32_t instanceCount;
	uint32_t firstIndex;
	int32_t vertexOffset;
	uint32_t firstInstance;
	void execute(VkCommandBuffer commandBuffer)
	{
		vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}
};

struct DispatchCommand
{
	uint32_t groupCountX;
	uint32_t groupCountY;
	uint32_t groupCountZ;
	void execute(VkCommandBuffer commandBuffer)
	{
		vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
	}
};

struct BarrierCommandSet
{
	struct BarrierCommand
	{
		VkPipelineStageFlags srcStageMask;
		VkPipelineStageFlags dstStageMask;
		VkDependencyFlags dependencyFlags;
		uint32_t startBufferMemoryBarrier;
		uint32_t bufferMemoryBarrierCount;
		uint32_t startImageMemoryBarrier;
		uint32_t imageMemoryBarrierCount;
	};
	std::vector<VkBufferMemoryBarrier> bufferBarriers;
	std::vector<VkImageMemoryBarrier> imageBarriers;
	std::vector<BarrierCommand> barrierCommands;
	void execute(VkCommandBuffer commandBuffer)
	{
		for (auto& pbc : barrierCommands)
		{
			GAIA_ASSERT(0 == pbc.bufferMemoryBarrierCount || pbc.bufferMemoryBarrierCount + pbc.startBufferMemoryBarrier <= bufferBarriers.size());
			GAIA_ASSERT(0 == pbc.imageMemoryBarrierCount || pbc.imageMemoryBarrierCount + pbc.startImageMemoryBarrier <= imageBarriers.size());
			vkCmdPipelineBarrier(
				commandBuffer,
				pbc.srcStageMask,
				pbc.dstStageMask,
				pbc.dependencyFlags,
				0,
				nullptr,
				pbc.bufferMemoryBarrierCount,
				pbc.bufferMemoryBarrierCount ? bufferBarriers.data() + pbc.startBufferMemoryBarrier : nullptr,
				pbc.imageMemoryBarrierCount,
				pbc.imageMemoryBarrierCount ? imageBarriers.data() + pbc.startImageMemoryBarrier : nullptr);
		}
	}
};


struct SimplePipelineCommand
{
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
	VkDescriptorSet descriptorSets[gaia_max_bind_group_count];
	uint32_t descriptorSetCount;
	VkPipelineBindPoint bindPoint;

	void execute(VkCommandBuffer commandBuffer)
	{
		vkCmdBindPipeline(commandBuffer, bindPoint, pipeline);
		if (descriptorSetCount)
		{
			vkCmdBindDescriptorSets(
				commandBuffer,
				bindPoint,
				pipelineLayout,
				0,
				descriptorSetCount,
				descriptorSets,
				0,
				nullptr);
		}
	}
};


END_GAIA_VK1