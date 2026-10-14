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
	constexpr uint32_t invalid_pass = UINT32_MAX;
	constexpr uint32_t invalid_resource = RenderGraph::invalid_resource;

	enum class ResourceAccessMode
	{
		read,
		write,
		read_write,
	};

	struct ResourceUsage
	{
		bool isWritten = false;
		bool isRead = false;
		uint32_t firstUsePass = invalid_pass;
		uint32_t lastUsePass = invalid_pass;
	public:
		void markWritten(uint32_t pass)
		{
			isWritten = true;
			updateUsePass(pass);
		}
		void markRead(uint32_t pass)
		{
			isRead = true;
			updateUsePass(pass);
		}
	private:
		void updateUsePass(uint32_t pass)
		{
			if (invalid_pass == firstUsePass || pass < firstUsePass)
			{
				firstUsePass = pass;
			}
			if (invalid_pass == lastUsePass || pass > lastUsePass)
			{
				lastUsePass = pass;
			}
		}
	};


	std::vector<ResourceUsage> CheckResources(
		RenderGraphCompiler::DiagnosticInfo& diagnosticInfo,
		const RenderGraph* renderGraph)
	{
		uint32_t resourceCount = renderGraph->resourceCount();

		//check duplicate external resource
		std::unordered_map<void*, uint32_t> externalResourceMap;
		for (uint32_t resourceIndex = 0; resourceIndex < resourceCount; ++resourceIndex)
		{
			auto* resource = renderGraph->resource(resourceIndex);
			if (resource->isExternal())
			{
				auto* externalResource = static_cast<RenderGraph::ExternalResource*>(resource);
				auto it = externalResourceMap.find(externalResource->resource());
				if (it != externalResourceMap.end())
				{
					RenderGraphCompiler::DiagnosticItem error;
					error.code = RenderGraphCompiler::DiagnosticCode::error_duplicate_external_resource;
					error.resource = resourceIndex;
					diagnosticInfo.errors.push_back(error);
				}
				else
				{
					externalResourceMap[externalResource->resource()] = resourceIndex;
				}
			}
		}

		//collect resoure usages
		std::vector<ResourceUsage> resourceUsages(resourceCount);

		uint32_t passCount = renderGraph->passCount();
		for (uint32_t passIndex = 0; passIndex < passCount; ++passIndex)
		{
			std::vector<const RenderGraph::ResourceBinding*> resourceBindings;
			RenderGraph::Pass* pass = renderGraph->pass(passIndex);
			pass->collectResourceBindings(resourceBindings);
			for (auto binding : resourceBindings)
			{
				uint32_t resource = binding->resource;
				if (resource >= renderGraph->resourceCount())
				{
					RenderGraphCompiler::DiagnosticItem error;
					error.code = RenderGraphCompiler::DiagnosticCode::error_resource_out_of_range;
					error.resource = resource;
					error.pass = passIndex;
					diagnosticInfo.errors.push_back(error);
					continue;
				}
				if (binding->isWrite())
				{
					RenderGraph::Resource* resourceInfo = renderGraph->resource(resource);
					resourceUsages[resource].markWritten(passIndex);
				}
				if (binding->isRead())
				{
					if (!renderGraph->resource(resource)->isExternal() && !resourceUsages[resource].isWritten)
					{
						RenderGraphCompiler::DiagnosticItem error;
						error.code = RenderGraphCompiler::DiagnosticCode::error_resource_read_before_write;
						error.resource = resource;
						error.pass = passIndex;
						diagnosticInfo.errors.push_back(error);
					}
					resourceUsages[resource].markRead(passIndex);
				}				
			}
		}

		//check missing access
		for (uint32_t resource = 0; resource < resourceCount; ++resource)
		{
			RenderGraph::Resource* resourceInfo = renderGraph->resource(resource);
			if (!resourceUsages[resource].isWritten)
			{
				if (!resourceInfo->isExternal())
				{
					RenderGraphCompiler::DiagnosticItem warning;
					warning.code = RenderGraphCompiler::DiagnosticCode::warning_resource_not_written;
					warning.resource = resource;
					diagnosticInfo.warnings.push_back(warning);
				}
			}
			if (!resourceUsages[resource].isRead)
			{
				if (!renderGraph->isOutputResource(resource))
				{
					RenderGraphCompiler::DiagnosticItem warning;
					warning.code = RenderGraphCompiler::DiagnosticCode::warning_resource_not_read;
					warning.resource = resource;
					diagnosticInfo.warnings.push_back(warning);
				}
			}
		}
		return resourceUsages;
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
		CompiledRenderGraph* compiledRenderGraph,
		const RenderGraph* renderGraph,
		const std::vector<ResourceUsage>& resourceUsages)
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

		for (uint32_t passIndex = 0; passIndex < passCount; ++passIndex)
		{
			auto* pass = renderGraph->pass(passIndex);
			for (uint32_t resource = 0; resource < resourceCount; ++resource)
			{
				const auto& usage = resourceUsages[resource];
				if (usage.firstUsePass != passIndex)
				{
					continue;
				}
				if (resourceToSlot[resource] != invalid_resource)
				{
					continue;
				}
				auto* info = renderGraph->resource(resource);
				if (info->isExternal())
				{
					uint32_t slot = compiledRenderGraph->cloneExternalResource(static_cast<RenderGraph::ExternalResource*>(info));
					resourceToSlot[resource] = slot;
					slotLifecycles.push_back({ passIndex, usage.lastUsePass, info->name() });
					continue;
				}

				if (info->kind() == RenderGraph::ResourceKind::texture)
				{
					auto* tex = static_cast<const RenderGraph::Texture*>(info);
					auto it = freeTextureByDesc.find(tex->desc());
					bool allocated = false;
					if (it != freeTextureByDesc.end())
					{
						auto& freeList = it->second;
						for (auto slotIt = freeList.begin(); slotIt != freeList.end(); ++slotIt)
						{
							uint32_t slot = *slotIt;
							if (usage.firstUsePass > slotLifecycles[slot].lastUsePass)
							{
								resourceToSlot[resource] = slot;
								slotLifecycles[slot].firstUsePass = std::min(slotLifecycles[slot].firstUsePass, usage.firstUsePass);
								slotLifecycles[slot].lastUsePass = std::max(slotLifecycles[slot].lastUsePass, usage.lastUsePass);
								slotLifecycles[slot].name += "," + info->name();
								freeList.erase(slotIt);
								allocated = true;
								break;
							}
						}
					}
					if (!allocated)
					{
						uint32_t slot = compiledRenderGraph->addTexture(tex->name(), tex->desc());
						resourceToSlot[resource] = slot;
						slotLifecycles.push_back({ usage.firstUsePass, usage.lastUsePass, info->name() });
					}
				}
				else if (info->kind() == RenderGraph::ResourceKind::buffer)
				{
					auto* buf = static_cast<const RenderGraph::Buffer*>(info);
					auto it = freeBufferByDesc.find(buf->desc());
					bool allocated = false;
					if (it != freeBufferByDesc.end())
					{
						auto& freeList = it->second;
						for (auto slotIt = freeList.begin(); slotIt != freeList.end(); ++slotIt)
						{
							uint32_t slot = *slotIt;
							if (usage.firstUsePass > slotLifecycles[slot].lastUsePass)
							{
								resourceToSlot[resource] = slot;
								slotLifecycles[slot].firstUsePass = std::min(slotLifecycles[slot].firstUsePass, usage.firstUsePass);
								slotLifecycles[slot].lastUsePass = std::max(slotLifecycles[slot].lastUsePass, usage.lastUsePass);
								slotLifecycles[slot].name += "," + info->name();
								freeList.erase(slotIt);
								allocated = true;
								break;
							}
						}
					}
					if (!allocated)
					{
						uint32_t slot = compiledRenderGraph->addBuffer(buf->name(), buf->desc());
						resourceToSlot[resource] = slot;
						slotLifecycles.push_back({ usage.firstUsePass, usage.lastUsePass, info->name() });
					}
				}
			}
			for (uint32_t resource = 0; resource < resourceCount; ++resource)
			{
				const auto& usage = resourceUsages[resource];
				if (usage.lastUsePass != passIndex) continue;
				if (resourceToSlot[resource] == invalid_resource) continue;

				uint32_t slot = resourceToSlot[resource];
				auto* info = renderGraph->resource(resource);
				compiledRenderGraph->resource(slot)->name() = slotLifecycles[slot].name;

				if (info->kind() == RenderGraph::ResourceKind::texture)
				{
					auto* tex = static_cast<const RenderGraph::Texture*>(info);
					freeTextureByDesc[tex->desc()].push_back(slot);
				}
				else if (info->kind() == RenderGraph::ResourceKind::buffer)
				{
					auto* buf = static_cast<const RenderGraph::Buffer*>(info);
					freeBufferByDesc[buf->desc()].push_back(slot);
				}
			}
		}
		return resourceToSlot;
	}

	void BuildCompiledPasses(
		CompiledRenderGraph* compiledRenderGraph,
		const RenderGraph* renderGraph,
		const std::vector<uint32_t>& executionOrder,
		const std::vector<uint32_t>& resourceToSlot)
	{
		uint32_t passCount = (uint32_t)executionOrder.size();

		for (uint32_t order = 0; order < passCount; ++order)
		{
			uint32_t passIndex = executionOrder[order];
			auto* srcPass = renderGraph->pass(passIndex);

			switch (srcPass->kind())
			{
			case RenderPassKind::scene_pass:
			{
				auto* srcScenePass = static_cast<RenderGraph::ScenePass*>(srcPass);
				auto* dstScenePass = compiledRenderGraph->addScenePass(srcScenePass->name());

				for (auto& rtv : srcScenePass->rtvs())
				{
					dstScenePass->addRTV(resourceToSlot[rtv.resource], rtv.usageDesc);
				}
				if (srcScenePass->dsv().resource != invalid_resource)
				{
					dstScenePass->setDSV(resourceToSlot[srcScenePass->dsv().resource], srcScenePass->dsv().usageDesc);
				}
				break;
			}
			case RenderPassKind::image_pass:
			{
				auto* srcImagePass = static_cast<RenderGraph::ImagePass*>(srcPass);
				auto* dstImagePass = compiledRenderGraph->addImagePass(srcImagePass->name());

				for (auto& srv : srcImagePass->srvs())
				{
					dstImagePass->addSRV(resourceToSlot[srv.resource], srv.usageDesc);
				}
				for (auto& rtv : srcImagePass->rtvs())
				{
					dstImagePass->addRTV(resourceToSlot[rtv.resource], rtv.usageDesc);
				}
				dstImagePass->pipeline(srcImagePass->pipeline());
				break;
			}
			case RenderPassKind::compute_pass:
			{
				auto* srcComputePass = static_cast<RenderGraph::ComputePass*>(srcPass);
				auto* dstComputePass = compiledRenderGraph->addComputePass(srcComputePass->name());

				for (auto& srv : srcComputePass->srvs())
				{
					dstComputePass->addSRV(resourceToSlot[srv.resource], srv.usageDesc);
				}
				for (auto& uav : srcComputePass->uavs())
				{
					dstComputePass->addUAV(resourceToSlot[uav.resource], uav.usageDesc);
				}
				dstComputePass->pipeline(srcComputePass->pipeline());
				break;
			}
			case RenderPassKind::ray_tracing_pass:
			{
				auto* srcRayTracingPass = static_cast<RenderGraph::RayTracingPass*>(srcPass);
				auto* dstRayTracingPass = compiledRenderGraph->addRayTracingPass(srcRayTracingPass->name());

				for (auto& srv : srcRayTracingPass->srvs())
				{
					dstRayTracingPass->addSRV(resourceToSlot[srv.resource], srv.usageDesc);
				}
				for (auto& uav : srcRayTracingPass->uavs())
				{
					dstRayTracingPass->addUAV(resourceToSlot[uav.resource], uav.usageDesc);
				}
				dstRayTracingPass->pipeline(srcRayTracingPass->pipeline());
				break;
			}
			default:
				GAIA_ASSERT(false && "Unknown pass kind");
				break;
			}
		}
	}

	void BuildCompiledBarriers(
		CompiledRenderGraph* output,
		const RenderGraph* input,
		const std::vector<uint32_t>& executionOrder,
		const PassToResource& passToResource,
		const std::vector<uint32_t>& resourceToSlot)
	{
		uint32_t passCount = (uint32_t)executionOrder.size();
		uint32_t resourceCount = input->resourceCount();
		uint32_t dstResourceCount = output->resourceCount();

		// 每个 slot 的当前状态
		std::vector<ResourceState> currentState(output->resourceCount(), ResourceState::undefined);

		for (uint32_t slot = 0; slot < dstResourceCount; ++slot)
		{
			auto* compiledResource = output->resource(slot);
			if (compiledResource->isExternal())
			{
				auto* ext = static_cast<const CompiledRenderGraph::ExternalResource*>(compiledResource);
				currentState[slot] = ext->oldState();
			}
		}

		for (uint32_t order = 0; order < passCount; ++order)
		{
			uint32_t passIndex = executionOrder[order];
			auto* outputPass = output->pass(order);

			// 读取：需要 shader_resource
			for (uint32_t resource : passToResource.items[passIndex].readResources)
			{
				uint32_t slot = resourceToSlot[resource];
				if (currentState[slot] != ResourceState::shader_resource)
				{
					outputPass->barriers().push_back({
						slot, currentState[slot], ResourceState::shader_resource
						});
					currentState[slot] = ResourceState::shader_resource;
				}
			}

			// 写入：需要目标状态
			for (uint32_t resource : passToResource.items[passIndex].writeResources)
			{
				uint32_t slot = resourceToSlot[resource];
				ResourceState target = GetTargetState(input->resource(resource), outputPass);
				if (currentState[slot] != target)
				{
					outputPass->barriers().push_back({
						slot, currentState[slot], target
						});
					currentState[slot] = target;
				}
			}
		}
	}

}


RenderGraphCompiler::DiagnosticInfo RenderGraphCompiler::compile(CompiledRenderGraph* compiledRenderGraph, const RenderGraph* renderGraph)
{
	RenderGraphCompiler::DiagnosticInfo diagnosticInfo;

	compiledRenderGraph->reset();
	uint32_t passCount = renderGraph->passCount();
	uint32_t resourceCount = renderGraph->resourceCount();

	std::vector<ResourceUsage> resourceUsages = CheckResources(diagnosticInfo, renderGraph);

	if (!diagnosticInfo.errors.empty())
	{
		return diagnosticInfo;
	}

	std::vector<uint32_t> resourceToSlot = BuildCompiledResources(compiledRenderGraph, renderGraph, resourceUsages);

	BuildCompiledPasses(compiledRenderGraph, renderGraph, executionOrder, resourceToSlot);

	return diagnosticInfo;
}

END_GAIA
