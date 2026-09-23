#include "render_graph_compiler.h"
#include "render_pass.h"
#include <unordered_map>
#include <unordered_set>
#include <queue>

BEGIN_GAIA

namespace
{
	bool CheckResourceValid(
		std::vector<RenderGraphCompiler::ErrorInfo>& errorInfos,
		RenderGraph* renderGraph,
		uint32_t resource,
		uint32_t pass)
	{
		if (resource >= (uint32_t)renderGraph->m_resources.size())
		{
			RenderGraphCompiler::ErrorInfo errorInfo;
			errorInfo.errorCode = RenderGraphErrorCode::invalid_resource;
			errorInfo.resource = resource;
			errorInfo.pass = pass;
			errorInfos.push_back(errorInfo);
			return false;
		}
		return true;
	}

	void MarkResourceRead(
		std::vector<RenderGraphCompiler::ErrorInfo>& errorInfos,
		std::unordered_map<uint32_t, std::unordered_set<uint32_t>>& resourceReadPasses,
		RenderGraph* renderGraph,
		uint32_t resource,
		uint32_t  pass)
	{
		if(CheckResourceValid(errorInfos, renderGraph, resource, pass))
		{
			resourceReadPasses[resource].insert(pass);
		}
	}

	void MarkResourceWrite(
		std::vector<RenderGraphCompiler::ErrorInfo>& errorInfos,
		std::unordered_map<uint32_t, uint32_t>& resourceWritePass,
		RenderGraph* renderGraph,
		uint32_t resource,
		uint32_t pass)
	{
		if(CheckResourceValid(errorInfos, renderGraph, resource, pass))
		{
			RenderGraph::ResourceInfo* resourceInfo = renderGraph->m_resources[resource];
			if (RenderGraph::ResourceKind::external_srv == resourceInfo->kind ||
				RenderGraph::ResourceKind::external_uav == resourceInfo->kind)
			{
				RenderGraphCompiler::ErrorInfo errorInfo;
				errorInfo.errorCode = RenderGraphErrorCode::external_resource_written;
				errorInfo.resource = resource;
				errorInfo.pass = pass;
				errorInfos.push_back(errorInfo);
			}
			else if (!resourceWritePass.insert(std::make_pair(resource, pass)).second)
			{
				RenderGraphCompiler::ErrorInfo errorInfo;
				errorInfo.errorCode = RenderGraphErrorCode::resource_multi_written;
				errorInfo.resource = resource;
				errorInfo.pass = pass;
				errorInfos.push_back(errorInfo);
			}
		}
	}

	void CheckResourceWritten(
		std::vector<RenderGraphCompiler::ErrorInfo>& errorInfos,
		std::unordered_map<uint32_t, uint32_t>& resourceWritePass,
		RenderGraph* renderGraph,
		uint32_t resource)
	{
		RenderGraph::ResourceInfo* resourceInfo = renderGraph->m_resources[resource];
		if (RenderGraph::ResourceKind::external_srv != resourceInfo->kind &&
			RenderGraph::ResourceKind::external_uav != resourceInfo->kind &&
			resourceWritePass.find(resource) == resourceWritePass.end())
		{
			RenderGraphCompiler::ErrorInfo errorInfo;
			errorInfo.errorCode = RenderGraphErrorCode::resource_not_written;
			errorInfo.resource = resource;
			errorInfos.push_back(errorInfo);
		}
	}

	void CheckExecutionOrder(
		std::vector<RenderGraphCompiler::ErrorInfo>& errorInfos,
		std::vector<uint32_t>& executionOrder,
		uint32_t passCount)
	{
		if (executionOrder.size() != passCount)
		{
			std::vector<bool> inOrder(passCount, false);
			for (uint32_t pass : executionOrder)
			{
				inOrder[pass] = true;
			}
			for (uint32_t pass = 0; pass < passCount; ++pass)
			{
				if (!inOrder[pass])
				{
					RenderGraphCompiler::ErrorInfo errorInfo;
					errorInfo.errorCode = RenderGraphErrorCode::pass_cyclic_dependency;
					errorInfo.pass = pass;
					errorInfos.push_back(errorInfo);
				}
			}
		}
	}
}

std::vector<RenderGraphCompiler::ErrorInfo> RenderGraphCompiler::compile(RenderGraph* renderGraph)
{
	std::vector<RenderGraphCompiler::ErrorInfo> errorInfos;

	//collect resource usage
	std::unordered_map<uint32_t, uint32_t> resourceWritePass;
	std::unordered_map<uint32_t, std::unordered_set<uint32_t>> resourceReadPasses;

	uint32_t passCount = renderGraph->m_renderPasses.size();
	for (uint32_t pass = 0; pass < passCount; ++pass)
	{
		RenderPass* renderPass = renderGraph->m_renderPasses[pass];
		switch (renderPass->kind())
		{
		case RenderPassKind::scene_pass:
			MarkResourceWrite(errorInfos, resourceWritePass, renderGraph, static_cast<ScenePass*>(renderPass)->m_dsv, pass);
			for (uint32_t rtv : static_cast<ScenePass*>(renderPass)->m_rtvs)
			{
				MarkResourceWrite(errorInfos, resourceWritePass, renderGraph, rtv, pass);
			}
			break;
		case RenderPassKind::image_pass:
			for (uint32_t srv : static_cast<ImagePass*>(renderPass)->m_srvs)
			{
				MarkResourceRead(errorInfos, resourceReadPasses, renderGraph, srv, pass);
			}
			for (uint32_t rtv : static_cast<ImagePass*>(renderPass)->m_rtvs)
			{
				MarkResourceWrite(errorInfos, resourceWritePass, renderGraph, rtv, pass);
			}
			break;
		case RenderPassKind::compute_pass: 
			for (uint32_t srv : static_cast<ComputePass*>(renderPass)->m_srvs)
			{
				MarkResourceRead(errorInfos, resourceReadPasses, renderGraph, srv, pass);
			}
			for (uint32_t uav : static_cast<ComputePass*>(renderPass)->m_uavs)
			{
				MarkResourceWrite(errorInfos, resourceWritePass, renderGraph, uav, pass);
			}
			break;
		case RenderPassKind::ray_tracing_pass:
			for (uint32_t srv : static_cast<RayTracingPass*>(renderPass)->m_srvs)
			{
				MarkResourceRead(errorInfos, resourceReadPasses, renderGraph, srv, pass);
			}
			for (uint32_t uav : static_cast<RayTracingPass*>(renderPass)->m_uavs)
			{
				MarkResourceWrite(errorInfos, resourceWritePass, renderGraph, uav, pass);
			}
			break;
		}
	}

	uint32_t resourceCount = (uint32_t)renderGraph->m_resources.size();
	for (uint32_t resource = 0; resource < resourceCount; ++resource)
	{
		CheckResourceWritten(errorInfos, resourceWritePass, renderGraph, resource);
	}

	if (!errorInfos.empty())
	{
		return errorInfos;
	}

	//build pass dependency
	std::vector<std::vector<uint32_t>> dependencies(passCount);
	for (auto& [resource, readers] : resourceReadPasses)
	{
		auto it = resourceWritePass.find(resource);
		GAIA_ASSERT(resourceWritePass.end() != it);
		uint32_t writeIndex = it->second;
		for (uint32_t readerIndex : readers)
		{
			dependencies[writeIndex].push_back(readerIndex);
		}
	}

	//build execution order
	m_executionOrder.clear();
	m_executionOrder.reserve(passCount);

	std::vector<uint32_t> inDegree(passCount, 0);
	for (uint32_t i = 0; i < passCount; ++i)
	{
		for (uint32_t downstream : dependencies[i])
		{
			inDegree[downstream]++;
		}
	}
	std::queue<uint32_t> ready;
	for (uint32_t i = 0; i < passCount; ++i)
	{
		if (inDegree[i] == 0)
		{
			ready.push(i);
		}
	}
	while (!ready.empty())
	{
		uint32_t current = ready.front();
		ready.pop();
		m_executionOrder.push_back(current);
		for (uint32_t downstream : dependencies[current])
		{
			if (--inDegree[downstream] == 0)
			{
				ready.push(downstream);
			}
		}
	}
	CheckExecutionOrder(errorInfos, m_executionOrder, passCount);

}

END_GAIA