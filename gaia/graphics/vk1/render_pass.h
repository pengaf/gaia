#pragma once
#include "utility.h"
#include "common.h"
#include <vector>

BEGIN_GAIA_VK1

class RenderPass
{
public:
	RenderPassKind kind() const
	{
		return m_kind;
	}
protected:
	RenderPassKind m_kind;
};

class ScenePass
{
public:
	void begin(VkCommandBuffer commandBuffer)
	{
		m_barrierSet.execute(commandBuffer);
		m_beginRendering.execute(commandBuffer);
	}
	void end(VkCommandBuffer commandBuffer)
	{
		vkCmdEndRendering(commandBuffer);
	}
public:
	BarrierCommandSet m_barrierSet;
	BeginRenderingCommand m_beginRendering;
};

class ImagePass
{
public:
	void execute(VkCommandBuffer commandBuffer)
	{
		m_barrierSet.execute(commandBuffer);
		m_beginRendering.execute(commandBuffer);
		m_bindPipeline.execute(commandBuffer);
		m_bindDescriptorSets.execute(commandBuffer);
		m_draw.execute(commandBuffer);
		vkCmdEndRendering(commandBuffer);
	}
public:
	BarrierCommandSet m_barrierSet;
	BeginRenderingCommand m_beginRendering;
	BindPipelineCommand m_bindPipeline;
	BindDescriptorSetsCommand m_bindDescriptorSets;
	DrawCommand m_draw;
};

class ComputePass
{
public:
	void execute(VkCommandBuffer commandBuffer)
	{
		m_barrierSet.execute(commandBuffer);
		m_bindPipeline.execute(commandBuffer);
		m_bindDescriptorSets.execute(commandBuffer);
		m_Dispatch.execute(commandBuffer);
	}
public:
	BarrierCommandSet m_barrierSet;
	BindPipelineCommand m_bindPipeline;
	BindDescriptorSetsCommand m_bindDescriptorSets;
	DispatchCommand m_Dispatch;
};


END_GAIA_VK1