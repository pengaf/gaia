#pragma once

#include "render_graph.h"

BEGIN_GAIA

class CompiledRenderGraph
{
public:
	struct Barrier
	{
		uint32_t resource;
		ResourceState oldState;
		ResourceState newState;
	};

	class Pass
	{
	public:
		Pass(std::string_view name, RenderPassKind kind) : m_name(name), m_kind(kind) {}
		virtual ~Pass() = default;
	public:
		AITL_ARG_RO(RenderPassKind, kind);
		AITL_ARG_RO(std::string, name);
		AITL_ARG(std::vector<Barrier>, barriers);
	};

	class ScenePass : public Pass
	{
		friend class RenderGraphCompiler;
	public:
		ScenePass(std::string_view name) : Pass(name, RenderPassKind::scene_pass) {}
	public:
		void addRTV(uint32_t rtv);
		void setDSV(uint32_t dsv);
	public:
		AITL_ARG_RO(std::vector<uint32_t>, rtvs);
		AITL_ARG_RO(uint32_t, dsv);
	};

	class ImagePass : public Pass
	{
		friend class RenderGraphCompiler;
	public:
		ImagePass(std::string_view name) : Pass(name, RenderPassKind::image_pass) {}
	public:
		void addSRV(uint32_t srv);
		void addRTV(uint32_t rtv);
		void setPipeline(GraphicsPipelineState* pipeline);
	public:
		AITL_ARG_RO(std::vector<uint32_t>, srvs);
		AITL_ARG_RO(std::vector<uint32_t>, rtvs);
	protected:
		GraphicsPipelineState* m_pipeline;
	};

	class ComputePass : public Pass
	{
		friend class RenderGraphCompiler;
	public:
		ComputePass(std::string_view name) : Pass(name, RenderPassKind::compute_pass) {}
	public:
		void addSRV(uint32_t srv);
		void addUAV(uint32_t uav);
		void setPipeline(ComputePipelineState* pipeline);
	public:
		AITL_ARG_RO(std::vector<uint32_t>, srvs);
		AITL_ARG_RO(std::vector<uint32_t>, uavs);
	protected:
		ComputePipelineState* m_pipeline;
	};

	class RayTracingPass : public Pass
	{
		friend class RenderGraphCompiler;
	public:
		RayTracingPass(std::string_view name) : Pass(name, RenderPassKind::ray_tracing_pass) {}
	public:
		void addSRV(uint32_t srv);
		void addUAV(uint32_t uav);
		void setPipeline(RayTracingPipelineState* pipeline);
	public:
		AITL_ARG_RO(std::vector<uint32_t>, srvs);
		AITL_ARG_RO(std::vector<uint32_t>, uavs);
	protected:
		RayTracingPipelineState* m_pipeline;
	};

public:
	using ResourceKind = RenderGraph::ResourceKind;
	using ViewKind = RenderGraph::ViewKind;

	struct Resource
	{
		ResourceKind kind;
		std::string name;
		Resource(ResourceKind kind, std::string_view name) : kind(kind), name(name) {}
		virtual ~Resource() = default;
	};

	struct Texture : Resource
	{
		TextureDesc desc;
		Texture(std::string_view name, const TextureDesc& desc) :Resource(ResourceKind::texture, name), desc(desc) {}
	};

	struct Buffer : Resource
	{
		BufferDesc desc;
		Buffer(std::string_view name, const BufferDesc& desc) :Resource(ResourceKind::buffer, name), desc(desc) {}
	};

	struct ExternalResource : Resource
	{
		union
		{
			ShaderResourceView* srv;
			UnorderedAccessView* uav;
			RenderTargetView* rtv;
			SwapChainView* scv;
			void* view;
		};
		ViewKind viewKind;
		ResourceState oldState;
		ExternalResource(std::string_view name, ViewKind viewKind, void* view, ResourceState oldState) : Resource(ResourceKind::external, name), view(view), viewKind(viewKind), oldState(oldState) {}
		ExternalResource(std::string_view name, ShaderResourceView* srv, ResourceState oldState) : Resource(ResourceKind::external, name), srv(srv), viewKind(ViewKind::srv), oldState(oldState) {}
		ExternalResource(std::string_view name, UnorderedAccessView* uav, ResourceState oldState) : Resource(ResourceKind::external, name), uav(uav), viewKind(ViewKind::uav), oldState(oldState) {}
		ExternalResource(std::string_view name, RenderTargetView* rtv, ResourceState oldState) : Resource(ResourceKind::external, name), rtv(rtv), viewKind(ViewKind::rtv), oldState(oldState) {}
		ExternalResource(std::string_view name, SwapChainView* scv, ResourceState oldState) : Resource(ResourceKind::external, name), scv(scv), viewKind(ViewKind::scv), oldState(oldState) {}
	};

public:
	uint32_t addTexture(std::string_view name, const TextureDesc& desc);
	uint32_t addBuffer(std::string_view name, const BufferDesc& desc);
	uint32_t addExternalResource(std::string_view name, ViewKind viewKind, void* view, ResourceState oldState);
	uint32_t addExternalSrv(std::string_view name, ShaderResourceView* srv, ResourceState oldState);
	uint32_t addExternalUav(std::string_view name, UnorderedAccessView* uav, ResourceState oldState);
	uint32_t addExternalRtv(std::string_view name, RenderTargetView* rtv, ResourceState oldState);
	uint32_t addExternalScv(std::string_view name, SwapChainView* scv, ResourceState oldState);
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
	std::vector< std::unique_ptr<Resource>> m_resources;
	std::vector<std::unique_ptr<Pass>> m_passes;
private:
	template<typename T>
	T* addPass(std::string_view name);
	template<typename T, class...Args>
	uint32_t addResource(Args&&... args);
};

END_GAIA