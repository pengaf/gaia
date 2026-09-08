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


class RenderGraph
{
public:

private:
	std::vector<TextureDesc> m_textures;
	std::vector<BufferDesc> m_buffers;
	std::vector<RenderPass*> m_renderPasses;
	pafcore::StringPool m_resourceNamePool;
	pafcore::StringPool m_passNamePool;
};

END_GAIA_RG