#pragma once

#include "utility.h"
#include "render_graph.h"
#include <unordered_map>
#include <unordered_set>

BEGIN_GAIA

class RenderGraph;

class RenderGraphCompiler
{
	struct ErrorInfo
	{
		RenderGraphErrorCode errorCode = RenderGraphErrorCode::ok;
		uint32_t resource = uint32_t(-1);
		RenderPass* renderPass = nullptr;
	};
public:
	ErrorInfo compile(RenderGraph* renderGraph);
protected:
	std::unordered_map<uint32_t, RenderPass*> m_resoureWritePass;
	std::unordered_map<uint32_t, std::unordered_set<RenderPass*>> m_resoureReadPasses;
};

END_GAIA