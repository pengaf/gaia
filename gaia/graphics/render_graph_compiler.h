#pragma once

#include "utility.h"
#include <vector>

BEGIN_GAIA

class RenderGraph;
class CompiledRenderGraph;

class RenderGraphCompiler
{
public:
	enum ErrorCode
	{
		invalid_resource,
		resource_not_written,
		resource_multi_written,
		external_resource_written,
		duplicate_external_resource,
		pass_cyclic_dependency,
	};
	struct ErrorInfo
	{
		ErrorCode errorCode;
		uint32_t resource = UINT32_MAX;
		uint32_t pass = UINT32_MAX;
	};
public:
	std::vector<ErrorInfo> compile(CompiledRenderGraph* compiledRenderGraph, const RenderGraph* renderGraph);
protected:
	//std::unordered_map<uint32_t, uint32_t> m_resourceWritePass;
	//std::unordered_map<uint32_t, std::unordered_set<uint32_t>> m_resourceReadPasses;
	std::vector<uint32_t> m_executionOrder;
	//std::vector<ResourceLifecycle> m_lifecycles;
	//std::vector<AliasingGroup> m_aliasingPlan;
	//std::vector<BarrierPlan> m_barrierPlan;  // 抽象状态};
};

END_GAIA
