#pragma once

#include "utility.h"
#include "common.h"
#include <vector>
#include "pafcore/pool.h"

BEGIN_GAIA_RG

class RenderPass;
class ScenePass;
class ImagePass;
class ComputePass;
class RayTracingPass;

class RenderGraphBuilder
{
public:
	TextureHandle createTexture(const TextureDesc& desc);
	BufferHandle createBuffer(const BufferDesc& desc);
	ScenePass* addScenePass(const char* name);
	ImagePass* addImagePass(const char* name);
	ComputePass* addComputePass(const char* name);
	RayTracingPass* addRayTracingPass(const char* name);
private:
	template<typename T>
	T* addPass(const char* name);
};

END_GAIA_RG