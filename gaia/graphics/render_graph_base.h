#pragma once

#include "utility.h"
#include "common.h"
#include <vector>
#include <string>
#include <memory>

BEGIN_GAIA

class GraphicsPipelineState;
class ComputePipelineState;
class RayTracingPipelineState;
class Texture;
class Buffer;

class RenderGraphBase
{
public:
	static constexpr uint32_t invalid_resource = UINT32_MAX;

	enum class ResourceKind
	{
		texture,
		buffer,
		external_texture,
		external_buffer,
	};

	class Resource
	{
	public:
		Resource(ResourceKind kind, std::string_view name) : m_kind(kind), m_name(name) {}
		virtual ~Resource() = default;
	public:
		bool isExternal() const
		{
			return m_kind == ResourceKind::external_texture || m_kind == ResourceKind::external_buffer;
		}
	public:
		GAIA_ARG_RO(ResourceKind, kind);
		GAIA_ARG_RO(std::string, name);
	};

	class Texture : public Resource
	{
	public:
		Texture(std::string_view name, const TextureDesc& desc) :Resource(ResourceKind::texture, name), m_desc(desc) {}
	public:
		GAIA_ARG_RO(TextureDesc, desc);
	};

	class Buffer : public Resource
	{
	public:
		Buffer(std::string_view name, const BufferDesc& desc) :Resource(ResourceKind::buffer, name), m_desc(desc) {}
	public:
		GAIA_ARG_RO(BufferDesc, desc);
	};

	class ExternalResource : public Resource
	{
	public:
		ExternalResource(std::string_view name, Texture* texture, ResourceState oldState) : Resource(ResourceKind::external_texture, name), m_texture(texture), m_oldState(oldState) {}
		ExternalResource(std::string_view name, Buffer* buffer, ResourceState oldState) : Resource(ResourceKind::external_buffer, name), m_buffer(buffer), m_oldState(oldState) {}
	public:
		void* resource() const
		{
			return m_resource;
		}
	public:
		GAIA_ARG_RO(ResourceState, oldState);
	private:
		union
		{
			Texture* m_texture;
			Buffer* m_buffer;
			void* m_resource;
		};
	};

public:

	enum class ResourceBindingKind
	{
		render_target,
		depth_stencil,
		shader_resource,
		unordered_access,
		copy_src,
		copy_dst,
		resolve_src,
		resolve_dst,
		blit_src,
		blit_dst,
		mipmap_src,
		mipmap_dst,
	};

	struct ResourceBinding
	{
		uint32_t resource;
		ResourceBindingKind usageKind;
	public:
		ResourceBinding(ResourceBindingKind kind) : resource(invalid_resource), usageKind(kind) {};
		ResourceBinding(uint32_t resource, ResourceBindingKind kind) : resource(resource), usageKind(kind) {};

		bool isRead() const
		{
			switch (usageKind)
			{
			case ResourceBindingKind::shader_resource:
			case ResourceBindingKind::copy_src:
			case ResourceBindingKind::resolve_src:
			case ResourceBindingKind::mipmap_src:
				return true;
			}
			return false;
		}

		bool isWrite() const
		{
			switch (usageKind)
			{
			case ResourceBindingKind::render_target:
			case ResourceBindingKind::depth_stencil:
			case ResourceBindingKind::unordered_access:
			case ResourceBindingKind::copy_dst:
			case ResourceBindingKind::resolve_dst:
			case ResourceBindingKind::blit_dst:
			case ResourceBindingKind::mipmap_dst:
				return true;
			}
			return false;
		}
	};

	template<ResourceBindingKind t_kind, typename Desc_t>
	struct ResourceBinding_t : ResourceBinding
	{
		Desc_t usageDesc;
		ResourceBinding_t() : ResourceBinding(t_kind) {}
		ResourceBinding_t(uint32_t resource, const Desc_t& desc) : ResourceBinding(resource, t_kind), usageDesc(desc) {}
	};

	using RTV = ResourceBinding_t<ResourceBindingKind::render_target, RenderTargetViewDesc>;
	using DSV = ResourceBinding_t<ResourceBindingKind::depth_stencil, DepthStencilViewDesc>;
	using SRV = ResourceBinding_t<ResourceBindingKind::shader_resource, ShaderResourceViewDesc>;
	using UAV = ResourceBinding_t<ResourceBindingKind::unordered_access, UnorderedAccessViewDesc>;

	struct Barrier
	{
		uint32_t resource;
		ResourceBinding* oldUsage;
		ResourceBinding* newUsage;
	};

	template<bool t_compiled>
	class PassBarriers
	{};

	template<>
	class PassBarriers<true>
	{
	public:
		GAIA_ARG_RO(std::vector<Barrier>, barriers);
	};

	template<bool t_compiled>
	class Pass : public PassBarriers<t_compiled>
	{
	public:
		Pass(std::string_view name, RenderPassKind kind) : m_name(name), m_kind(kind) {}
		virtual ~Pass() = default;
		virtual void collectResourceBindings(std::vector<const ResourceBinding*>& resourceBindings) const = 0;
	public:
		GAIA_ARG_RO(RenderPassKind, kind);
		GAIA_ARG_RO(std::string, name);
	};

	//template<bool t_compiled>
	//class CopyPass : public Pass<t_compiled>
	//{
	//public:
	//	CopyPass(std::string_view name) : Pass<t_compiled>(name, RenderPassKind::copy_pass) {}
	//public:
	//	void setSrc(uint32_t resource)
	//	{
	//		m_src = { resource, desc };
	//	}
	//	void setDst(uint32_t resource)
	//	{
	//		m_dst = { resource, desc };
	//	}
	//	void setCopyDesc(const ResourceCopyDesc& desc)
	//	{
	//		m_copyDesc = desc;
	//	}
	//public:
	//	GAIA_ARG(uint32_t, src);
	//	GAIA_ARG(uint32_t, dst);
	//	GAIA_ARG(ResourceCopyDesc, desc);
	//};

	template<bool t_compiled>
	class ScenePass : public Pass<t_compiled>
	{
	public:
		ScenePass(std::string_view name) : Pass<t_compiled>(name, RenderPassKind::scene_pass) {}
	public:
		void addRTV(uint32_t resource, const RenderTargetViewDesc& desc)
		{
			m_rtvs.push_back({ resource, desc });
		}
		void setDSV(uint32_t resource, const DepthStencilViewDesc& desc)
		{
			m_dsv = { resource, desc };
		}
		virtual void collectResourceBindings(std::vector<const ResourceBinding*>& resourceBindings) const override
		{
			for (auto& rtv : m_rtvs)
			{
				resourceBindings.push_back(&rtv);
			}
			if (invalid_resource != m_dsv.resource)
			{
				resourceBindings.push_back(&m_dsv);
			}
		}
	public:
		GAIA_ARG_RO(std::vector<RTV>, rtvs);
		GAIA_ARG_RO(DSV, dsv);
	};

	template<bool t_compiled>
	class ImagePass : public Pass<t_compiled>
	{
	public:
		ImagePass(std::string_view name) : Pass<t_compiled>(name, RenderPassKind::image_pass) {}
	public:
		void addSRV(uint32_t resource, const ShaderResourceViewDesc& desc)
		{
			m_srvs.push_back({ resource, desc });
		}
		void addRTV(uint32_t resource, const RenderTargetViewDesc& desc)
		{
			m_rtvs.push_back({ resource, desc });
		}
		virtual void collectResourceBindings(std::vector<const ResourceBinding*>& resourceBindings) const override
		{
			for (auto& srv : m_srvs)
			{
				resourceBindings.push_back(&srv);
			}
			for (auto& rtv : m_rtvs)
			{
				resourceBindings.push_back(&rtv);
			}
		}
	public:
		GAIA_ARG_RO(std::vector<SRV>, srvs);
		GAIA_ARG_RO(std::vector<RTV>, rtvs);
		GAIA_ARG_PTR(GraphicsPipelineState, pipeline);
	};

	template<bool t_compiled>
	class ComputePass : public Pass<t_compiled>
	{
	public:
		ComputePass(std::string_view name) : Pass<t_compiled>(name, RenderPassKind::compute_pass) {}
	public:
		void addSRV(uint32_t resource, const ShaderResourceViewDesc& desc)
		{
			m_srvs.push_back({ resource, desc });
		}
		void addUAV(uint32_t resource, const UnorderedAccessViewDesc& desc)
		{
			m_uavs.push_back({ resource, desc });
		}
		virtual void collectResourceBindings(std::vector<const ResourceBinding*>& resourceBindings) const override
		{
			for (auto& srv : m_srvs)
			{
				resourceBindings.push_back(&srv);
			}
			for (auto& uav : m_uavs)
			{
				resourceBindings.push_back(&uav);
			}
		}
	public:
		GAIA_ARG_RO(std::vector<SRV>, srvs);
		GAIA_ARG_RO(std::vector<UAV>, uavs);
		GAIA_ARG_PTR(ComputePipelineState, pipeline);
	};

	template<bool t_compiled>
	class RayTracingPass : public Pass<t_compiled>
	{
	public:
		RayTracingPass(std::string_view name) : Pass<t_compiled>(name, RenderPassKind::ray_tracing_pass) {}
	public:
		void addSRV(uint32_t resource, const ShaderResourceViewDesc& desc)
		{
			m_srvs.push_back({ resource, desc });
		}
		void addUAV(uint32_t resource, const UnorderedAccessViewDesc& desc)
		{
			m_uavs.push_back({ resource, desc });
		}
		virtual void collectResourceBindings(std::vector<const ResourceBinding*>& resourceBindings) const override
		{
			for (auto& srv : m_srvs)
			{
				resourceBindings.push_back(&srv);
			}
			for (auto& uav : m_uavs)
			{
				resourceBindings.push_back(&uav);
			}
		}
	public:
		GAIA_ARG_RO(std::vector<SRV>, srvs);
		GAIA_ARG_RO(std::vector<UAV>, uavs);
		GAIA_ARG_PTR(RayTracingPipelineState, pipeline);
	};
};

END_GAIA