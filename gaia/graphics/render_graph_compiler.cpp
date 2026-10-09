#include "render_graph_compiler.h"
#include "render_graph.h"
#include "compiled_render_graph.h"
#include <unordered_map>
#include <unordered_set>
#include "pafcore/flat_set.h"
#include <queue>

BEGIN_GAIA

inline uint64_t fnv1a_64(const uint8_t* data, size_t len, uint64_t hash = 0xcbf29ce484222325ull)
{
	const uint64_t fnv64_prime = 0x00000100000001b3ull;
	for (size_t i = 0; i < len; ++i)
	{
		hash ^= data[i];
		hash *= fnv64_prime;
	}
	return hash;
}

template<typename T>
inline uint64_t fnv1a_64(const T& value, uint64_t hash = 0xcbf29ce484222325ull)
{
	return fnv1a_64(reinterpret_cast<const uint8_t*>(&value), sizeof(T), hash);
}

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
		const RenderGraph* renderGraph)
	{
		std::unordered_map<void*, uint32_t> viewMap;
		uint32_t resourceCount = (uint32_t)renderGraph->m_resources.size();
		for (uint32_t i = 0; i < resourceCount; ++i)
		{
			auto* info = renderGraph->m_resources[i].get();
			if (info->kind == RenderGraph::ResourceKind::external)
			{
				auto* ext = static_cast<RenderGraph::ExternalResource*>(info);
				auto it = viewMap.find(ext->view);
				if (it != viewMap.end())
				{
					RenderGraphCompiler::ErrorInfo errorInfo;
					errorInfo.errorCode = RenderGraphCompiler::ErrorCode::duplicate_external_resource;
					errorInfo.resource = i;
					errorInfos.push_back(errorInfo);
				}
				else
				{
					viewMap[ext->view] = i;
				}
			}
		}
	}

	bool CheckResourceValid(
		std::vector<RenderGraphCompiler::ErrorInfo>& errorInfos,
		const RenderGraph* renderGraph,
		uint32_t resource,
		uint32_t pass)
	{
		if (resource >= (uint32_t)renderGraph->m_resources.size())
		{
			RenderGraphCompiler::ErrorInfo errorInfo;
			errorInfo.errorCode = RenderGraphCompiler::ErrorCode::invalid_resource;
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
		const RenderGraph* renderGraph,
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
		const RenderGraph* renderGraph,
		uint32_t resource,
		uint32_t pass)
	{
		if (CheckResourceValid(errorInfos, renderGraph, resource, pass))
		{
			RenderGraph::Resource* resourceInfo = renderGraph->resource(resource);
			if (RenderGraph::ResourceKind::external == resourceInfo->kind)
			{
				RenderGraphCompiler::ErrorInfo errorInfo;
				errorInfo.errorCode = RenderGraphCompiler::ErrorCode::external_resource_written;
				errorInfo.resource = resource;
				errorInfo.pass = pass;
				errorInfos.push_back(errorInfo);
			}
			else if (!resourceToPass.setWritePass(resource, pass))
			{
				RenderGraphCompiler::ErrorInfo errorInfo;
				errorInfo.errorCode = RenderGraphCompiler::ErrorCode::resource_multi_written;
				errorInfo.resource = resource;
				errorInfo.pass = pass;
				errorInfos.push_back(errorInfo);
			}
		}
	}


	void CheckResourceWritten(
		std::vector<RenderGraphCompiler::ErrorInfo>& errorInfos,
		const ResourceToPass& resourceToPass,
		const RenderGraph* renderGraph,
		uint32_t resource)
	{
		RenderGraph::Resource* resourceInfo = renderGraph->resource(resource);
		if (RenderGraph::ResourceKind::external != resourceInfo->kind && !resourceToPass.IsWritten(resource))
		{
			RenderGraphCompiler::ErrorInfo errorInfo;
			errorInfo.errorCode = RenderGraphCompiler::ErrorCode::resource_not_written;
			errorInfo.resource = resource;
			errorInfos.push_back(errorInfo);
		}
	}

	ResourceToPass BuildResourceToPass(
		std::vector<RenderGraphCompiler::ErrorInfo>& errorInfos,
		const RenderGraph* renderGraph)
	{
		uint32_t passCount = renderGraph->passCount();
		uint32_t resourceCount = renderGraph->resourceCount();
		ResourceToPass resourceToPass(resourceCount);
		for (uint32_t passIndex = 0; passIndex < passCount; ++passIndex)
		{
			RenderGraph::Pass* pass = renderGraph->pass(passIndex);
			switch (pass->kind())
			{
			case RenderPassKind::scene_pass:
				MarkResourceWrite(errorInfos, resourceToPass, renderGraph, static_cast<RenderGraph::ScenePass*>(pass)->dsv(), passIndex);
				for (uint32_t rtv : static_cast<RenderGraph::ScenePass*>(pass)->rtvs())
				{
					MarkResourceWrite(errorInfos, resourceToPass, renderGraph, rtv, passIndex);
				}
				break;
			case RenderPassKind::image_pass:
				for (uint32_t srv : static_cast<RenderGraph::ImagePass*>(pass)->srvs())
				{
					MarkResourceRead(errorInfos, resourceToPass, renderGraph, srv, passIndex);
				}
				for (uint32_t rtv : static_cast<RenderGraph::ImagePass*>(pass)->rtvs())
				{
					MarkResourceWrite(errorInfos, resourceToPass, renderGraph, rtv, passIndex);
				}
				break;
			case RenderPassKind::compute_pass:
				for (uint32_t srv : static_cast<RenderGraph::ComputePass*>(pass)->srvs())
				{
					MarkResourceRead(errorInfos, resourceToPass, renderGraph, srv, passIndex);
				}
				for (uint32_t uav : static_cast<RenderGraph::ComputePass*>(pass)->uavs())
				{
					MarkResourceWrite(errorInfos, resourceToPass, renderGraph, uav, passIndex);
				}
				break;
			case RenderPassKind::ray_tracing_pass:
				for (uint32_t srv : static_cast<RenderGraph::RayTracingPass*>(pass)->srvs())
				{
					MarkResourceRead(errorInfos, resourceToPass, renderGraph, srv, passIndex);
				}
				for (uint32_t uav : static_cast<RenderGraph::RayTracingPass*>(pass)->uavs())
				{
					MarkResourceWrite(errorInfos, resourceToPass, renderGraph, uav, passIndex);
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
					errorInfo.errorCode = RenderGraphCompiler::ErrorCode::pass_cyclic_dependency;
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

	struct BufferDescHash
	{
		size_t operator()(const BufferDesc& desc) const
		{
			uint64_t h = fnv1a_64(desc.size);
			h = fnv1a_64(desc.bufferUsage, h);
			h = fnv1a_64(desc.cpuAccess, h);
			return (size_t)h;
		}
	};

	struct TextureDescHash
	{
		size_t operator()(const TextureDesc& desc) const
		{
			uint64_t h = fnv1a_64(desc.width);
			h = fnv1a_64(desc.height, h);
			h = fnv1a_64(desc.depth, h);
			h = fnv1a_64(desc.mipLevels, h);
			h = fnv1a_64(desc.arrayLayers, h);
			h = fnv1a_64(desc.dimension, h);
			h = fnv1a_64(desc.format, h);
			h = fnv1a_64(desc.textureUsage, h);
			h = fnv1a_64(desc.cpuAccess, h);
			return (size_t)h;
		}
	};

	std::vector<uint32_t> BuildCompiledResources(
		CompiledRenderGraph* output,
		const RenderGraph* renderGraph,
		const std::vector<uint32_t>& executionOrder,
		const PassToResource& passToResource,
		const ResourceLifecycle& lifecycles)
	{
		uint32_t passCount = renderGraph->passCount();
		uint32_t resourceCount = renderGraph->resourceCount();
		std::vector<uint32_t> resourceToSlot(resourceCount, invalid_resource);

		struct AliasingSlot
		{
			uint32_t firstUsePass = invalid_pass;
			uint32_t lastUsePass = invalid_pass;
			std::string name;
		};
		std::vector<AliasingSlot> slotLifecycles;

		std::unordered_map<TextureDesc, std::vector<uint32_t>, TextureDescHash> freeTextureByDesc;
		std::unordered_map<BufferDesc, std::vector<uint32_t>, BufferDescHash> freeBufferByDesc;

		for (uint32_t order = 0; order < passCount; ++order)
		{
			uint32_t pass = executionOrder[order];

			for (uint32_t resource : passToResource.items[pass].writeResources)
			{
				if (resourceToSlot[resource] != invalid_resource)
				{
					continue;
				}
				auto* info = renderGraph->resource(resource);
				if (info->kind == RenderGraph::ResourceKind::external)
				{
					auto* ext = static_cast<RenderGraph::ExternalResource*>(info);
					uint32_t slot = output->addExternalResource(ext->name, ext->viewKind, ext->view, ext->oldState);
					resourceToSlot[resource] = slot;
					slotLifecycles.push_back({ invalid_pass, invalid_pass, info->name });
				}
				else if (info->kind == RenderGraph::ResourceKind::texture)
				{
					auto* tex = static_cast<RenderGraph::Texture*>(info);
					auto it = freeTextureByDesc.find(tex->desc);
					bool allocated = false;
					if (it != freeTextureByDesc.end())
					{
						auto& freeList = it->second;
						for (auto slotIt = freeList.begin(); slotIt != freeList.end(); ++slotIt)
						{
							uint32_t slot = *slotIt;
							if( lifecycles.items[resource].firstUsePass > slotLifecycles[slot].lastUsePass ||
								lifecycles.items[resource].lastUsePass < slotLifecycles[slot].firstUsePass)
							{
								resourceToSlot[resource] = slot;
								slotLifecycles[slot].firstUsePass = std::min(slotLifecycles[slot].firstUsePass, lifecycles.items[resource].firstUsePass);
								slotLifecycles[slot].lastUsePass = std::max(slotLifecycles[slot].lastUsePass, lifecycles.items[resource].lastUsePass);
								slotLifecycles[slot].name += "," + info->name;
								freeList.erase(slotIt);
								allocated = true;
								break;
							}
						}
					}
					if (!allocated)
					{
						uint32_t slot = output->addTexture(tex->name, tex->desc);
						resourceToSlot[resource] = slot;
						slotLifecycles.push_back({lifecycles.items[resource].firstUsePass, lifecycles.items[resource].lastUsePass, info->name });
					}
				}
				else if (info->kind == RenderGraph::ResourceKind::buffer)
				{
					auto* buf = static_cast<RenderGraph::Buffer*>(info);
					auto it = freeBufferByDesc.find(buf->desc);
					bool allocated = false;
					if (it != freeBufferByDesc.end())
					{
						auto& freeList = it->second;
						for (auto slotIt = freeList.begin(); slotIt != freeList.end(); ++slotIt)
						{
							uint32_t slot = *slotIt;
							if (lifecycles.items[resource].firstUsePass > slotLifecycles[slot].lastUsePass ||
								lifecycles.items[resource].lastUsePass < slotLifecycles[slot].firstUsePass)
							{
								resourceToSlot[resource] = slot;
								slotLifecycles[slot].firstUsePass = std::min(slotLifecycles[slot].firstUsePass, lifecycles.items[resource].firstUsePass);
								slotLifecycles[slot].lastUsePass = std::max(slotLifecycles[slot].lastUsePass, lifecycles.items[resource].lastUsePass);
								slotLifecycles[slot].name += "," + info->name;
								freeList.erase(slotIt);
								allocated = true;
								break;
							}
						}
					}
					if (!allocated)
					{
						uint32_t slot = output->addBuffer(buf->name, buf->desc);
						resourceToSlot[resource] = slot;
						slotLifecycles.push_back({ lifecycles.items[resource].firstUsePass, lifecycles.items[resource].lastUsePass, info->name });
					}
				}
			}

			auto releaseResource = [&](uint32_t resource)
				{
					if (lifecycles.items[resource].lastUsePass == pass && resourceToSlot[resource] != invalid_resource)
					{
						uint32_t slot = resourceToSlot[resource];
						auto* info = renderGraph->resource(resource);
						output->m_resources[slot]->name = slotLifecycles[slot].name;

						if (info->kind == RenderGraph::ResourceKind::texture)
						{
							auto* tex = static_cast<RenderGraph::Texture*>(info);
							freeTextureByDesc[tex->desc].push_back(slot);
						}
						else if (info->kind == RenderGraph::ResourceKind::buffer)
						{
							auto* buf = static_cast<RenderGraph::Buffer*>(info);
							freeBufferByDesc[buf->desc].push_back(slot);
						}
					}
				};

			for (uint32_t resource : passToResource.items[pass].writeResources)
			{
				releaseResource(resource);
			}
			for (uint32_t resource : passToResource.items[pass].readResources)
			{
				releaseResource(resource);
			}
		}
		return resourceToSlot;
	}
}

std::vector<RenderGraphCompiler::ErrorInfo> RenderGraphCompiler::compile(CompiledRenderGraph* compiledRenderGraph, const RenderGraph* renderGraph)
{
	compiledRenderGraph->reset();

	std::vector<RenderGraphCompiler::ErrorInfo> errorInfos;
	uint32_t passCount = renderGraph->passCount();
	uint32_t resourceCount = renderGraph->resourceCount();

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

	std::vector<uint32_t> resourceToSlot = BuildCompiledResources(compiledRenderGraph, renderGraph, executionOrder, passToResource, resourceLifecycle);

	return errorInfos;
}

END_GAIA
