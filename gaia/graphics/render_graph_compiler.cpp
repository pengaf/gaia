#include "render_graph_compiler.h"
#include "render_pass.h"
#include <unordered_map>
#include <unordered_set>
#include <queue>

BEGIN_GAIA

namespace
{
	struct ResourceAccess
	{
		std::unordered_map<uint32_t, uint32_t> resourceWritePass;
		std::unordered_map<uint32_t, std::unordered_set<uint32_t>> resourceReadPasses;
		std::vector<std::vector<uint32_t>> passWrites;
		std::vector<std::vector<uint32_t>> passReads;
	};


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
		uint32_t pass)
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

	ResourceAccess BuildResourceAccessInfo(
		std::vector<RenderGraphCompiler::ErrorInfo>& errorInfos,
		RenderGraph* renderGraph)
	{
		ResourceAccess resourceAccess;
		uint32_t passCount = renderGraph->m_renderPasses.size();
		for (uint32_t pass = 0; pass < passCount; ++pass)
		{
			RenderPass* renderPass = renderGraph->m_renderPasses[pass];
			switch (renderPass->kind())
			{
			case RenderPassKind::scene_pass:
				MarkResourceWrite(errorInfos, resourceAccess.resourceWritePass, renderGraph, static_cast<ScenePass*>(renderPass)->dsv(), pass);
				for (uint32_t rtv : static_cast<ScenePass*>(renderPass)->rtvs())
				{
					MarkResourceWrite(errorInfos, resourceAccess.resourceWritePass, renderGraph, rtv, pass);
				}
				break;
			case RenderPassKind::image_pass:
				for (uint32_t srv : static_cast<ImagePass*>(renderPass)->srvs())
				{
					MarkResourceRead(errorInfos, resourceAccess.resourceReadPasses, renderGraph, srv, pass);
				}
				for (uint32_t rtv : static_cast<ImagePass*>(renderPass)->rtvs())
				{
					MarkResourceWrite(errorInfos, resourceAccess.resourceWritePass, renderGraph, rtv, pass);
				}
				break;
			case RenderPassKind::compute_pass:
				for (uint32_t srv : static_cast<ComputePass*>(renderPass)->srvs())
				{
					MarkResourceRead(errorInfos, resourceAccess.resourceReadPasses, renderGraph, srv, pass);
				}
				for (uint32_t uav : static_cast<ComputePass*>(renderPass)->uavs())
				{
					MarkResourceWrite(errorInfos, resourceAccess.resourceWritePass, renderGraph, uav, pass);
				}
				break;
			case RenderPassKind::ray_tracing_pass:
				for (uint32_t srv : static_cast<RayTracingPass*>(renderPass)->srvs())
				{
					MarkResourceRead(errorInfos, resourceAccess.resourceReadPasses, renderGraph, srv, pass);
				}
				for (uint32_t uav : static_cast<RayTracingPass*>(renderPass)->uavs())
				{
					MarkResourceWrite(errorInfos, resourceAccess.resourceWritePass, renderGraph, uav, pass);
				}
				break;
			}
		}

		uint32_t resourceCount = (uint32_t)renderGraph->m_resources.size();
		for (uint32_t resource = 0; resource < resourceCount; ++resource)
		{
			CheckResourceWritten(errorInfos, resourceAccess.resourceWritePass, renderGraph, resource);
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
		uint32_t passCount,
		const std::vector<uint32_t>& executionOrder)
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


	std::vector<std::vector<uint32_t>> BuildPassDependency(
		uint32_t passCount,
		const std::unordered_map<uint32_t, uint32_t>& resourceWritePass,
		const std::unordered_map<uint32_t, std::unordered_set<uint32_t>>& resourceReadPasses)
	{
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
		return dependencies;
	}


	std::vector<uint32_t> BuildExecutionOrder_v1(
		uint32_t passCount,
		const std::unordered_map<uint32_t, uint32_t>& resourceWritePass,
		const std::unordered_map<uint32_t, std::unordered_set<uint32_t>>& resourceReadPasses)
	{
		std::vector<std::vector<uint32_t>> dependencies = BuildPassDependency(passCount, resourceWritePass, resourceReadPasses);
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

		std::vector<uint32_t> order;
		order.reserve(passCount);

		while (!ready.empty())
		{
			uint32_t current = ready.front();
			ready.pop();
			order.push_back(current);
			for (uint32_t downstream : dependencies[current])
			{
				if (--inDegree[downstream] == 0)
				{
					ready.push(downstream);
				}
			}
		}
		return order;
	}


	std::vector<uint32_t> BuildEndedCount(
		uint32_t passCount,
		uint32_t resourceCount,
		const std::unordered_map<uint32_t, uint32_t>& resourceWritePass,
		const std::unordered_map<uint32_t, std::unordered_set<uint32_t>>& resourceReadPasses)
	{
		std::vector<uint32_t> lastUsePasses(resourceCount, UINT32_MAX);
		for (auto& [resource, producer] : resourceWritePass)
		{
			lastUsePasses[resource] = std::max(lastUsePasses[resource], producer);
		}
		for (auto& [resource, readers] : resourceReadPasses)
		{
			for (uint32_t reader : readers)
			{
				lastUsePasses[resource] = std::max(lastUsePasses[resource], reader);
			}
		}
		std::vector<uint32_t> endedCount(passCount, 0);
		for (uint32_t resource = 0; resource < resourceCount; ++resource)
		{
			uint32_t lastPass = lastUsePasses[resource];
			if (lastPass != UINT32_MAX) 
			{
				endedCount[lastPass]++;
			}
		}
		return endedCount;
	}

	std::vector<uint32_t> BuildExecutionOrder_v2(
		uint32_t passCount,
		uint32_t resourceCount,
		const std::unordered_map<uint32_t, uint32_t>& resourceWritePass,
		const std::unordered_map<uint32_t, std::unordered_set<uint32_t>>& resourceReadPasses)
	{
		std::vector<std::vector<uint32_t>> dependencies = BuildPassDependency(passCount, resourceWritePass, resourceReadPasses);

		std::vector<uint32_t> inDegree(passCount, 0);
		for (uint32_t i = 0; i < passCount; ++i) 
		{
			for (uint32_t downstream : dependencies[i]) 
			{
				inDegree[downstream]++;
			}
		}

		std::vector<uint32_t> endedCount = BuildEndedCount(passCount, resourceCount, resourceWritePass, resourceReadPasses);
		struct Compare
		{
			const std::vector<uint32_t>* endedCount;
			bool operator()(uint32_t a, uint32_t b) const 
			{
				return (*endedCount)[a] < (*endedCount)[b];
			}
		};
		Compare compare{ &endedCount };
		std::priority_queue<uint32_t, std::vector<uint32_t>, Compare> ready(compare);

		for (uint32_t i = 0; i < passCount; ++i) 
		{
			if (inDegree[i] == 0) ready.push(i);
		}

		std::vector<uint32_t> order;
		order.reserve(passCount);
		while (!ready.empty()) 
		{
			uint32_t current = ready.top();
			ready.pop();
			order.push_back(current);
			for (uint32_t downstream : dependencies[current]) 
			{
				if (--inDegree[downstream] == 0) {
					ready.push(downstream);
				}
			}
		}
		return order;
	}

	RenderGraphCompiler::ResourcePlan BuildResourcePlan(
		const std::vector<uint32_t>& executionOrder,
		const ResourceAccess& resourceAccessInfo)
	{
		uint32_t passCount = (uint32_t)executionOrder.size();
		std::vector<std::vector<uint32_t>> passWrites(passCount);
		std::vector<std::vector<uint32_t>> passReads(passCount);

		RenderGraphCompiler::ResourcePlan resourcePlan;

		for (uint32_t order = 0; order < passCount; ++order)
		{
			uint32_t pass = executionOrder[order];
		}
	}
}



std::vector<RenderGraphCompiler::ErrorInfo> RenderGraphCompiler::compile(RenderGraph* renderGraph)
{
	std::vector<RenderGraphCompiler::ErrorInfo> errorInfos;

	//collect resource usage
	std::unordered_map<uint32_t, uint32_t> resourceWritePass;
	std::unordered_map<uint32_t, std::unordered_set<uint32_t>> resourceReadPasses;


	if (!errorInfos.empty())
	{
		return errorInfos;
	}


	//std::vector<uint32_t> executionOrder = BuildExecutionOrder_v1(dependencies);
	std::vector<uint32_t> executionOrder = BuildExecutionOrder_v2(passCount, resourceCount, resourceWritePass, resourceReadPasses);
	CheckExecutionOrder(errorInfos, passCount, executionOrder);
	if (!errorInfos.empty())
	{
		return errorInfos;
	}


	return errorInfos;

}

END_GAIA
