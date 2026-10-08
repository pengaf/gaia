#pragma once

#include "utility.h"
#include "common.h"
#include <vector>
#include <string>

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
	duplicate_external_resource,
	pass_cyclic_dependency,
};

class RenderGraph
{
public:
	uint32_t addTexture(std::string_view name, const TextureDesc& desc);
	uint32_t addBuffer(std::string_view name, const BufferDesc& desc);
	uint32_t addExternalSrv(std::string_view name, ShaderResourceView* srv, ResourceState oldState);
	uint32_t addExternalUav(std::string_view name, UnorderedAccessView* uav, ResourceState oldState);
	ScenePass* addScenePass(std::string_view name);
	ImagePass* addImagePass(std::string_view name);
	ComputePass* addComputePass(std::string_view name);
	RayTracingPass* addRayTracingPass(std::string_view name);
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
		std::string name;
		ResourceInfo(ResourceKind kind, std::string_view name) : kind(kind), name(name) {}
	};
	
	struct TextureInfo : ResourceInfo
	{
		TextureDesc desc;
		TextureInfo(std::string_view name, const TextureDesc& desc) :ResourceInfo(ResourceKind::texture, name), desc(desc) {}
	};
	
	struct BufferInfo : ResourceInfo
	{
		BufferDesc desc;
		BufferInfo(std::string_view name, const BufferDesc& desc) :ResourceInfo(ResourceKind::buffer, name), desc(desc) {}
	};
	
	struct ExternalSrvInfo : ResourceInfo
	{
		ShaderResourceView* srv;
		ResourceState oldState;
		ExternalSrvInfo(std::string_view name, ShaderResourceView* srv, ResourceState oldState) : ResourceInfo(ResourceKind::external_srv, name), srv(srv), oldState(oldState){}
	};

	struct ExternalUavInfo : ResourceInfo
	{
		UnorderedAccessView* uav;
		ResourceState oldState;
		ExternalUavInfo(std::string_view name, UnorderedAccessView* uav, ResourceState oldState) : ResourceInfo(ResourceKind::external_uav, name), uav(uav), oldState(oldState) {}
	};
public:
	std::vector<ResourceInfo*> m_resources;
	std::vector<RenderPass*> m_renderPasses;
private:
	template<typename T>
	T* addPass(std::string_view name);
};

END_GAIA