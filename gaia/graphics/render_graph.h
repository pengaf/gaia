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

enum RenderGraphErrorCode
{
	ok,
	resource_not_written,
	resource_multi_written,
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
	struct ResourceInfo
	{
		const char* name;
		ResourceInfo(const char* name) : name(name) {}
	};
	struct TextureInfo : ResourceInfo, TextureDesc
	{
		TextureInfo(const char* name, const TextureDesc& desc) :ResourceInfo(name), TextureDesc(desc) {}
	};
	struct BufferInfo : ResourceInfo, BufferDesc
	{
		BufferInfo(const char* name, const BufferDesc& desc) :ResourceInfo(name), BufferDesc(desc) {}
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