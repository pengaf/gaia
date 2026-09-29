#pragma once

#include "utility.h"
#include "render_graph.h"
#include <vector>

BEGIN_GAIA

class RenderGraphCompiler
{
public:
	struct ErrorInfo
	{
		RenderGraphErrorCode errorCode;
		uint32_t resource = UINT32_MAX;
		uint32_t pass = UINT32_MAX;
	};
	struct ResourcePlan
	{
		std::vector<RenderGraph::ResourceInfo*> resources;
	public:
		ResourcePlan() = default;
		ResourcePlan(ResourcePlan&&) = default;
		uint32_t addTexture(const char* name, const TextureDesc& desc)
		{
		}
		uint32_t addBuffer(const char* name, const BufferDesc& desc)
		{
		}
		void remove
	};
public:
	std::vector<ErrorInfo> compile(RenderGraph* renderGraph);
protected:
	//std::unordered_map<uint32_t, uint32_t> m_resourceWritePass;
	//std::unordered_map<uint32_t, std::unordered_set<uint32_t>> m_resourceReadPasses;
	std::vector<uint32_t> m_executionOrder;
	//std::vector<ResourceLifecycle> m_lifecycles;
	//std::vector<AliasingGroup> m_aliasingPlan;
	//std::vector<BarrierPlan> m_barrierPlan;  // 抽象状态};
};

END_GAIA
