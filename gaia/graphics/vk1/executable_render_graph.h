#pragma once
#include "utility.h"
#include "common.h"
#include <vector>
#include <memory>

BEGIN_GAIA_VK1

class TextureHandle;
class BufferHandle;
class Texture;
class Buffer;

class ExecutableRenderGraph
{
public:
	class Pass
	{
	public:
		RenderPassKind kind() const
		{
			return m_kind;
		}
	protected:
		RenderPassKind m_kind;
	};

	class ScenePass : public Pass
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

	class ImagePass : public Pass
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

	class ComputePass : public Pass
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
public:
	std::vector<TextureHandle> m_textures;
	std::vector<BufferHandle> m_buffers;
	std::vector<RefPtr<Texture>> m_externalTextures;
	std::vector<RefPtr<Buffer>> m_externalBuffers;
	std::vector<VkImageView> m_imageViews;
	std::vector<std::unique_ptr<Pass>> m_passes;
};

END_GAIA_VK1
