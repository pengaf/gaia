#pragma once

#include "utility.h"
#include "common.h"
#include <vector>
#include "pafcore/pool.h"

BEGIN_GAIA

class RenderPass;
class ScenePass;
class ImagePass;
class ComputePass;
class RayTracingPass;
class ShaderResourceView;
class UnorderedAccessView;

enum RenderGraphErrorCode
{
	invalid_resource,
	resource_not_written,
	resource_multi_written,
	external_resource_written,
	pass_cyclic_dependency,
};

class RenderGraph
{
public:
	uint32_t addTexture(const char* name, const TextureDesc& desc);
	uint32_t addBuffer(const char* name, const BufferDesc& desc);
	ScenePass* addScenePass(const char* name);
	ImagePass* addImagePass(const char* name);
	ComputePass* addComputePass(const char* name);
	RayTracingPass* addRayTracingPass(const char* name);
public:
	enum class ResourceKind
	{
		texture,
		buffer,
		external_srv,
		external_uav,
	};
	struct ResourceInfo
	{
		ResourceKind kind;
		const char* name;
		ResourceInfo(ResourceKind kind, const char* name) : kind(kind), name(name) {}
	};
	struct TextureInfo : ResourceInfo
	{
		TextureDesc desc;
		TextureInfo(const char* name, const TextureDesc& desc) :ResourceInfo(ResourceKind::texture, name), desc(desc) {}
	};
	struct BufferInfo : ResourceInfo
	{
		BufferDesc desc;
		BufferInfo(const char* name, const BufferDesc& desc) :ResourceInfo(ResourceKind::buffer, name), desc(desc) {}
	};
	struct ExternalSrvInfo : ResourceInfo
	{
		ShaderResourceView* srv;
		ExternalSrvInfo(const char* name, ShaderResourceView* srv) : ResourceInfo(ResourceKind::external_srv, name), srv(srv) {}
	};
	struct ExternalUavInfo : ResourceInfo
	{
		UnorderedAccessView* uav;
		ExternalUavInfo(const char* name, UnorderedAccessView* uav) : ResourceInfo(ResourceKind::external_uav, name), uav(uav) {}
	};
	std::vector<ResourceInfo*> m_resources;
	std::vector<RenderPass*> m_renderPasses;
	pafcore::StringPool m_resourceNamePool;
	pafcore::StringPool m_passNamePool;
private:
	template<typename T>
	T* addPass(const char* name);
};

END_GAIA