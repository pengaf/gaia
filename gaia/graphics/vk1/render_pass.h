#pragma once
#include "utility.h"
#include "common.h"
#include <vector>

BEGIN_GAIA_VK1

class ImagePass : public BarrierCommand, public SimplePipelineCommand
{
public:
	void execute(VkCommandBuffer commandBuffer);
private:
	VkRenderingAttachmentInfo m_colorAttachmentInfos[gaia_max_color_attment_count];
	VkRenderingAttachmentInfo m_depthAttachmentInfo;
	VkRenderingAttachmentInfo m_stencilAttachmentInfo;
	VkRenderingInfo m_renderingInfo;
	DrawCommand m_drawCommand;
};

class ComputePass : public BarrierCommand, public SimplePipelineCommand
{
public:
	void execute(VkCommandBuffer commandBuffer);
private:
	DispatchCommand m_dispatchCommand;
};


END_GAIA_VK1