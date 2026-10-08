#include "render_graph_compiler.h"
#include "render_pass.h"
#include <unordered_map>
#include <unordered_set>
#include "pafcore/flat_set.h"
#include <queue>

BEGIN_GAIA

namespace
{

	const uint32_t invalid_resource = UINT32_MAX;
	const uint32_t invalid_pass = UINT32_MAX;

	struct ResourceToPass
	{
		struct Item
		{
			uint32_t writePass = invalid_pass;
			pafcore::FlatSet<uint32_t> readPasses;
		};
		std::vector<Item> items;
	public:
		ResourceToPass(uint32_t resourceCount) : items(resourceCount) 
		{};

		uint32_t resourceCount() const
		{
			return (uint32_t)items.size();
		}
		bool setWritePass(uint32_t resource, uint32_t pass)
		{
			GAIA_ASSERT(resource < resourceCount());
			if (invalid_pass == items[resource].writePass)
			{
				items[resource].writePass = pass;
				return true;
			}
			else
			{
				return false;
			}
		}
		void addReadPass(uint32_t resource, uint32_t pass)
		{
			GAIA_ASSERT(resource < resourceCount());
			items[resource].readPasses.insert(pass);
		}
		bool IsWritten(uint32_t resource) const
		{
			GAIA_ASSERT(resource < resourceCount());
			return invalid_pass != items[resource].writePass;
		}
		uint32_t writePass(uint32_t resource) const
		{
			return items[resource].writePass;
		}
		const pafcore::FlatSet<uint32_t>& readPasses(uint32_t resource) const
		{
			return items[resource].readPasses;
		}
	};

	struct PassToResource
	{
		struct Item
		{
			std::vector<uint32_t> writeResources;
			std::vector<uint32_t> readResources;
		};
		std::vector<Item> items;
	public:
		PassToResource(uint32_t passCount) : items(passCount) 
		{};
		uint32_t passCount() const
		{
			return (uint32_t)items.size();
		}
		void addWriteResource(uint32_t pass, uint32_t resource)
		{
			GAIA_ASSERT(pass < passCount());
			items[pass].writeResources.push_back(resource);
		}
		void addReadResource(uint32_t pass, uint32_t resource)
		{
			GAIA_ASSERT(pass < passCount());
			items[pass].readResources.push_back(resource);
		}
	};

	struct PassDependency
	{
		std::vector<pafcore::FlatSet<uint32_t>> items;
	public:
		PassDependency(uint32_t passCount) : items(passCount)
		{};
		uint32_t passCount() const
		{
			return (uint32_t)items.size();
		}
		void addDependency(uint32_t writePass, uint32_t readPass)
		{
			GAIA_ASSERT(writePass < passCount());
			items[writePass].insert(readPass);
		}
		const pafcore::FlatSet<uint32_t>& dependency(uint32_t writePass) const
		{
			GAIA_ASSERT(writePass < passCount());
			return items[writePass];
		}
	};

	struct ResourceLifecycle
	{
		struct Item
		{
			uint32_t firstUsePass = invalid_pass;
			uint32_t lastUsePass = invalid_pass;
		};
		std::vector<Item> items;
	public:
		ResourceLifecycle(uint32_t resourceCount) : items(resourceCount)
		{};
		uint32_t resourceCount() const
		{
			return (uint32_t)items.size();
		}
		void updateUsePass(uint32_t resource, uint32_t usePass)
		{
			GAIA_ASSERT(resource < resourceCount() && usePass != invalid_pass);
			Item& item = items[resource];
			if (invalid_pass == item.firstUsePass || usePass < item.firstUsePass)
			{
				item.firstUsePass = usePass;
			}
			if (invalid_pass == item.lastUsePass || usePass > item.lastUsePass)
			{
				item.lastUsePass = usePass;
			}
		}
	};

	void CheckExternalResourceDuplicates(
		std::vector<RenderGraphCompiler::ErrorInfo>& errorInfos,
		RenderGraph* renderGraph)
	{
		std::unordered_map<ShaderResourceView*, uint32_t> srvMap;
		std::unordered_map<UnorderedAccessView*, uint32_t> uavMap;
		uint32_t resourceCount = (uint32_t)renderGraph->m_resources.size();
		for (uint32_t i = 0; i < resourceCount; ++i)
		{
			auto* info = renderGraph->m_resources[i];

			if (info->kind == RenderGraph::ResourceKind::external_srv)
			{
				auto* srvInfo = static_cast<RenderGraph::ExternalSrvInfo*>(info);
				auto it = srvMap.find(srvInfo->srv);
				if (it != srvMap.end())
				{
					RenderGraphCompiler::ErrorInfo errorInfo;
					errorInfo.errorCode = RenderGraphErrorCode::duplicate_external_resource;
					errorInfo.resource = i;
					errorInfos.push_back(errorInfo);
				}
				else
				{
					srvMap[srvInfo->srv] = i;
				}
			}
			else if (info->kind == RenderGraph::ResourceKind::external_uav)
			{
				auto* uavInfo = static_cast<RenderGraph::ExternalUavInfo*>(info);
				auto it = uavMap.find(uavInfo->uav);
				if (it != uavMap.end())
				{
					RenderGraphCompiler::ErrorInfo errorInfo;
					errorInfo.errorCode = RenderGraphErrorCode::duplicate_external_resource;
					errorInfo.resource = i;
					errorInfos.push_back(errorInfo);
				}
				else
				{
					uavMap[uavInfo->uav] = i;
				}
			}
		}
	}

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
		ResourceToPass& resourceToPass,
		RenderGraph* renderGraph,
		uint32_t resource,
		uint32_t pass)
	{
		if (CheckResourceValid(errorInfos, renderGraph, resource, pass))
		{
			resourceToPass.addReadPass(resource, pass);
		}
	}

	void MarkResourceWrite(
		std::vector<RenderGraphCompiler::ErrorInfo>& errorInfos,
		ResourceToPass& resourceToPass,
		RenderGraph* renderGraph,
		uint32_t resource,
		uint32_t pass)
	{
		if (CheckResourceValid(errorInfos, renderGraph, resource, pass))
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
			else if (!resourceToPass.setWritePass(resource, pass))
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
		const ResourceToPass& resourceToPass,
		RenderGraph* renderGraph,
		uint32_t resource)
	{
		RenderGraph::ResourceInfo* resourceInfo = renderGraph->m_resources[resource];
		if (RenderGraph::ResourceKind::external_srv != resourceInfo->kind &&
			RenderGraph::ResourceKind::external_uav != resourceInfo->kind &&
			!resourceToPass.IsWritten(resource))
		{
			RenderGraphCompiler::ErrorInfo errorInfo;
			errorInfo.errorCode = RenderGraphErrorCode::resource_not_written;
			errorInfo.resource = resource;
			errorInfos.push_back(errorInfo);
		}
	}

	ResourceToPass BuildResourceToPass(
		std::vector<RenderGraphCompiler::ErrorInfo>& errorInfos,
		RenderGraph* renderGraph)
	{
		uint32_t passCount = renderGraph->m_renderPasses.size();
		uint32_t resourceCount = renderGraph->m_resources.size();
		ResourceToPass resourceToPass(resourceCount);
		for (uint32_t pass = 0; pass < passCount; ++pass)
		{
			RenderPass* renderPass = renderGraph->m_renderPasses[pass];
			switch (renderPass->kind())
			{
			case RenderPassKind::scene_pass:
				MarkResourceWrite(errorInfos, resourceToPass, renderGraph, static_cast<ScenePass*>(renderPass)->dsv(), pass);
				for (uint32_t rtv : static_cast<ScenePass*>(renderPass)->rtvs())
				{
					MarkResourceWrite(errorInfos, resourceToPass, renderGraph, rtv, pass);
				}
				break;
			case RenderPassKind::image_pass:
				for (uint32_t srv : static_cast<ImagePass*>(renderPass)->srvs())
				{
					MarkResourceRead(errorInfos, resourceToPass, renderGraph, srv, pass);
				}
				for (uint32_t rtv : static_cast<ImagePass*>(renderPass)->rtvs())
				{
					MarkResourceWrite(errorInfos, resourceToPass, renderGraph, rtv, pass);
				}
				break;
			case RenderPassKind::compute_pass:
				for (uint32_t srv : static_cast<ComputePass*>(renderPass)->srvs())
				{
					MarkResourceRead(errorInfos, resourceToPass, renderGraph, srv, pass);
				}
				for (uint32_t uav : static_cast<ComputePass*>(renderPass)->uavs())
				{
					MarkResourceWrite(errorInfos, resourceToPass, renderGraph, uav, pass);
				}
				break;
			case RenderPassKind::ray_tracing_pass:
				for (uint32_t srv : static_cast<RayTracingPass*>(renderPass)->srvs())
				{
					MarkResourceRead(errorInfos, resourceToPass, renderGraph, srv, pass);
				}
				for (uint32_t uav : static_cast<RayTracingPass*>(renderPass)->uavs())
				{
					MarkResourceWrite(errorInfos, resourceToPass, renderGraph, uav, pass);
				}
				break;
			}
		}

		for (uint32_t resource = 0; resource < resourceCount; ++resource)
		{
			CheckResourceWritten(errorInfos, resourceToPass, renderGraph, resource);
		}
		return resourceToPass;
	}

	PassToResource BuildPassToResource(
		uint32_t passCount,
		const ResourceToPass& resourceToPass)
	{
		PassToResource passToResource(passCount);
		uint32_t resourceCount = resourceToPass.resourceCount();

		for (uint32_t resource = 0; resource < resourceCount; ++resource)
		{
			uint32_t writePass = resourceToPass.writePass(resource);
			if (invalid_pass != writePass)
			{
				GAIA_ASSERT(writePass < passCount);
				passToResource.addWriteResource(writePass, resource);
			}
			for (uint32_t readPass : resourceToPass.readPasses(resource))
			{
				passToResource.addReadResource(readPass, resource);
			}
		}
		return passToResource;
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


	PassDependency BuildPassDependency(
		uint32_t passCount,
		const ResourceToPass& resourceToPass)
	{
		PassDependency passDependency(passCount);
		uint32_t resourceCount = resourceToPass.resourceCount();
		for (uint32_t resource = 0; resource < resourceCount; ++resource)
		{
			uint32_t writePass = resourceToPass.writePass(resource);
			if (invalid_pass != writePass)
			{
				for (uint32_t readPass : resourceToPass.readPasses(resource))
				{
					passDependency.addDependency(writePass, readPass);
				}
			}
		}
		return passDependency;
	}


	std::vector<uint32_t> BuildExecutionOrder_v1(
		uint32_t passCount,
		const ResourceToPass& resourceToPass)
	{
		PassDependency dependency = BuildPassDependency(passCount, resourceToPass);
		std::vector<uint32_t> inDegree(passCount, 0);
		for (uint32_t i = 0; i < passCount; ++i)
		{
			for (uint32_t downstream : dependency.dependency(i))
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
			for (uint32_t downstream : dependency.dependency(current))
			{
				if (--inDegree[downstream] == 0)
				{
					ready.push(downstream);
				}
			}
		}
		return order;
	}

	ResourceLifecycle BuildLifecycle(
		const ResourceToPass& resourceToPass)
	{
		uint32_t resourceCount = resourceToPass.resourceCount();
		ResourceLifecycle lifecycles(resourceCount);
		for (uint32_t resource = 0; resource < resourceCount; ++resource)
		{
			uint32_t writePass = resourceToPass.writePass(resource);
			if (writePass != invalid_pass)
			{
				lifecycles.updateUsePass(resource, writePass);
			}
			for (uint32_t readPass : resourceToPass.readPasses(resource))
			{
				lifecycles.updateUsePass(resource, readPass);
			}
		}
		return lifecycles;
	}

	std::vector<uint32_t> BuildExecutionOrder_v2(
		uint32_t passCount,
		const ResourceToPass& resourceToPass,
		const ResourceLifecycle& resourceLifecycle)
	{
		PassDependency dependency = BuildPassDependency(passCount, resourceToPass);

		std::vector<uint32_t> inDegree(passCount, 0);
		for (uint32_t i = 0; i < passCount; ++i) 
		{
			for (uint32_t downstream : dependency.dependency(i)) 
			{
				inDegree[downstream]++;
			}
		}

		uint32_t resourceCount = resourceToPass.resourceCount();
		std::vector<uint32_t> endedCount(passCount, 0);
		for (uint32_t resource = 0; resource < resourceCount; ++resource)
		{
			uint32_t lastPass = resourceLifecycle.items[resource].lastUsePass;
			if (lastPass != invalid_pass)
			{
				endedCount[lastPass]++;
			}
		}

		struct Compare
		{
			const std::vector<uint32_t>* endedCount;
			bool operator()(uint32_t lhs, uint32_t rhs) const 
			{
				return (*endedCount)[lhs] < (*endedCount)[rhs];
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
			for (uint32_t downstream : dependency.dependency(current)) 
			{
				if (--inDegree[downstream] == 0) {
					ready.push(downstream);
				}
			}
		}
		return order;
	}

	bool IsSameResource(const RenderGraph::ResourceInfo* lhs, const RenderGraph::ResourceInfo* rhs)
	{
		if (lhs->kind != rhs->kind)
		{
			return false;
		}
		if (lhs->kind == RenderGraph::ResourceKind::texture)
		{
			auto* lt = static_cast<const RenderGraph::TextureInfo*>(lhs);
			auto* rt = static_cast<const RenderGraph::TextureInfo*>(rhs);
			return lt->desc == rt->desc;
		}
		else if (lhs->kind == RenderGraph::ResourceKind::buffer)
		{
			auto* lb = static_cast<const RenderGraph::BufferInfo*>(lhs);
			auto* rb = static_cast<const RenderGraph::BufferInfo*>(rhs);
			return lb->desc == rb->desc;
		}
		return false;
	}


	RenderGraphCompiler::ResourcePlan BuildResourcePlan(
		uint32_t passCount,
		uint32_t resourceCount,
		const std::vector<uint32_t>& executionOrder,
		const ResourceToPass& resourceToPass, 
		const PassToResource& passToResource)
	{

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
	uint32_t passCount = renderGraph->m_renderPasses.size();
	uint32_t resourceCount = (uint32_t)renderGraph->m_resources.size();

	ResourceToPass resourceToPass = BuildResourceToPass(errorInfos, renderGraph);
	CheckExternalResourceDuplicates(errorInfos, renderGraph);
	if (!errorInfos.empty())
	{
		return errorInfos;
	}

	ResourceLifecycle resourceLifecycle = BuildLifecycle(resourceToPass);

	std::vector<uint32_t> executionOrder = BuildExecutionOrder_v2(passCount, resourceToPass, resourceLifecycle);
	CheckExecutionOrder(errorInfos, passCount, executionOrder);
	if (!errorInfos.empty())
	{
		return errorInfos;
	}

	PassToResource passToResource = BuildPassToResource(passCount, resourceToPass);


	return errorInfos;
}

END_GAIA
