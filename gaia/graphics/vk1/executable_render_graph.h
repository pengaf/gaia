#pragma once
#include "utility.h"
#include "common.h"
#include <vector>
#include <string>
#include <memory>

BEGIN_GAIA_VK1

class TextureHandle;
class BufferHandle;
class Texture;
class Buffer;
class RenderSystem;

class ExecutableRenderGraph
{
public:
	class Pass
	{
	public:
		GAIA_ARG_RO_VAL(RenderPassKind, kind);
		GAIA_ARG(std::string, name);
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
		BindPipelineCommand<VK_PIPELINE_BIND_POINT_GRAPHICS> m_bindPipeline;
		BindDescriptorSetsCommand<VK_PIPELINE_BIND_POINT_GRAPHICS> m_bindDescriptorSets;
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
			m_dispatch.execute(commandBuffer);
		}
	public:
		BarrierCommandSet m_barrierSet;
		BindPipelineCommand<VK_PIPELINE_BIND_POINT_COMPUTE> m_bindPipeline;
		BindDescriptorSetsCommand<VK_PIPELINE_BIND_POINT_COMPUTE> m_bindDescriptorSets;
		DispatchCommand m_dispatch;
	};

	class RayTracingPass : public Pass
	{
	public:
		void execute(VkCommandBuffer commandBuffer)
		{
			m_barrierSet.execute(commandBuffer);
			m_bindPipeline.execute(commandBuffer);
			m_bindDescriptorSets.execute(commandBuffer);
			m_traceRays.execute(commandBuffer);
		}
	public:
		BarrierCommandSet m_barrierSet;
		BindPipelineCommand<VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR> m_bindPipeline;
		BindDescriptorSetsCommand<VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR> m_bindDescriptorSets;
		TraceRaysCommand m_traceRays;
	};

public:
	void reset(RenderSystem* renderSystem);
	ScenePass* addScenePass(std::string_view name);
	ImagePass* addImagePass(std::string_view name);
	ComputePass* addComputePass(std::string_view name);
public:
	uint32_t passCount()const
	{
		return (uint32_t)m_passes.size();
	}
	Pass* pass(uint32_t index) const
	{
		GAIA_ASSERT(index < m_passes.size());
		return m_passes[index].get();
	}
public:
	std::vector<TextureHandle> m_textures;
	std::vector<BufferHandle> m_buffers;
	std::vector<RefPtr<Texture>> m_externalTextures;
	std::vector<RefPtr<Buffer>> m_externalBuffers;
	std::vector<VkImageView> m_imageViews;
	std::vector<std::unique_ptr<Pass>> m_passes;
private:
	template<typename T>
	T* addPass_(std::string_view name);
};

END_GAIA_VK1
