#pragma once

#include "render_graph_base.h"

BEGIN_GAIA

class RenderGraph : public RenderGraphBase
{
public:
	using Pass = RenderGraphBase::Pass<false>;
	using ScenePass = RenderGraphBase::ScenePass<false>;
	using ImagePass = RenderGraphBase::ImagePass<false>;
	using ComputePass = RenderGraphBase::ComputePass<false>;
	using RayTracingPass = RenderGraphBase::RayTracingPass<false>;
public:
	uint32_t addTexture(std::string_view name, const TextureDesc& desc);
	uint32_t addBuffer(std::string_view name, const BufferDesc& desc);
	uint32_t addExternalTexture(std::string_view name, Texture* texture, ResourceState oldState);
	uint32_t addExternalBuffer(std::string_view name, Buffer* buffer, ResourceState oldState);
	ScenePass* addScenePass(std::string_view name);
	ImagePass* addImagePass(std::string_view name);
	ComputePass* addComputePass(std::string_view name);
	RayTracingPass* addRayTracingPass(std::string_view name);
	void reset();
public:
	uint32_t resourceCount()const
	{
		return (uint32_t)m_resources.size();
	}
	uint32_t passCount()const
	{
		return (uint32_t)m_passes.size();
	}
	Resource* resource(uint32_t index) const
	{
		GAIA_ASSERT(index < m_resources.size());
		return m_resources[index].get();
	}
	Pass* pass(uint32_t index) const
	{
		GAIA_ASSERT(index < m_passes.size());
		return m_passes[index].get();
	}
public:
	std::vector<std::unique_ptr<Resource>> m_resources;
	std::vector<std::unique_ptr<Pass>> m_passes;
private:
	template<typename T>
	T* addPass(std::string_view name);
	template<typename T, class...Args>
	uint32_t addResource(Args&&... args);
};

END_GAIA