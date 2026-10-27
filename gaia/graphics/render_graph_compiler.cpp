#include "render_graph_compiler.h"
#include "render_graph.h"
#include "compiled_render_graph.h"
#include "pafcore/flat_set.h"
#include "pafcore/hash.h"
#include <unordered_map>
#include <queue>
#include <algorithm>

BEGIN_GAIA

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
				void* realResource = nullptr;
				if (resource->kind() == RenderGraph::ResourceKind::external_texture)
				{
					realResource = static_cast<RenderGraph::ExternalTexture*>(resource)->texture();
				}
				else if (resource->kind() == RenderGraph::ResourceKind::external_buffer)
				{
					realResource = static_cast<RenderGraph::ExternalBuffer*>(resource)->buffer();
				}
				if (nullptr == realResource)
				{
					RenderGraphCompiler::DiagnosticItem error;
					error.code = RenderGraphCompiler::DiagnosticCode::error_external_resource_null;
					error.resource = resourceIndex;
					diagnosticInfo.addError(error);
					continue;
				}
				auto it = externalResourceMap.find(realResource);
				if (it != externalResourceMap.end())
				{
					RenderGraphCompiler::DiagnosticItem error;
					error.code = RenderGraphCompiler::DiagnosticCode::error_duplicate_external_resource;
					error.resource = resourceIndex;
					diagnosticInfo.addError(error);
				}
				else
				{
					externalResourceMap[realResource] = resourceIndex;
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
			pafcore::FlatSet<uint32_t> checkedResources;
			for (auto binding : resourceBindings)
			{
				uint32_t resource = binding->resource;
				if (resource >= renderGraph->resourceCount())
				{
					RenderGraphCompiler::DiagnosticItem error;
					error.code = RenderGraphCompiler::DiagnosticCode::error_resource_out_of_range;
					error.resource = resource;
					error.pass = passIndex;
					diagnosticInfo.addError(error);
					continue;
				}
				if (checkedResources.find(resource) != checkedResources.end())
				{
					RenderGraphCompiler::DiagnosticItem error;
					error.code = RenderGraphCompiler::DiagnosticCode::error_duplicate_resource_in_pass;
					error.resource = resource;
					error.pass = passIndex;
					diagnosticInfo.addError(error);
				}
				else
				{
					checkedResources.insert(resource);
				}

				bool bindingIsTexture = RenderGraph::ResourceBindingIsTexture(binding);
				bool resourceIsTexture = renderGraph->resource(resource)->isTexture();
				if (bindingIsTexture != resourceIsTexture)
				{
					RenderGraphCompiler::DiagnosticItem error;
					error.code = RenderGraphCompiler::DiagnosticCode::error_resource_binding_mismatch;
					error.resource = resource;
					error.pass = passIndex;
					diagnosticInfo.addError(error);
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
						diagnosticInfo.addError(error);
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
					diagnosticInfo.addWarning(warning);
				}
			}
			if (!resourceUsages[resource].isRead)
			{
				if (!renderGraph->isOutputResource(resource))
				{
					RenderGraphCompiler::DiagnosticItem warning;
					warning.code = RenderGraphCompiler::DiagnosticCode::warning_resource_not_read;
					warning.resource = resource;
					diagnosticInfo.addWarning(warning);
				}
			}
		}
		return resourceUsages;
	}

	void CheckOutputResources(
		RenderGraphCompiler::DiagnosticInfo& diagnosticInfo,
		const RenderGraph* renderGraph,
		const std::vector<ResourceUsage>& resourceUsages)
	{
		uint32_t outputResourceCount = renderGraph->outputResourceCount();
		if (0 == outputResourceCount)
		{
			RenderGraphCompiler::DiagnosticItem error;
			error.code = RenderGraphCompiler::DiagnosticCode::error_output_resource_not_marked;
			diagnosticInfo.addError(error);
		}
		for (uint32_t i = 0; i < outputResourceCount; ++i)
		{
			uint32_t resource = renderGraph->outputResource(i);
			if (resource >= renderGraph->resourceCount())
			{
				RenderGraphCompiler::DiagnosticItem error;
				error.code = RenderGraphCompiler::DiagnosticCode::error_output_resource_out_of_range;
				error.resource = resource;
				diagnosticInfo.addError(error);
			}
			else
			{
				if (!resourceUsages[resource].isWritten)
				{
					RenderGraphCompiler::DiagnosticItem error;
					error.code = RenderGraphCompiler::DiagnosticCode::error_output_resource_not_written;
					error.resource = resource;
					diagnosticInfo.addError(error);
				}
			}
		}
	}

	void CheckPasses(
		RenderGraphCompiler::DiagnosticInfo& diagnosticInfo,
		const RenderGraph* renderGraph)
	{
		uint32_t passCount = renderGraph->passCount();
		for (uint32_t passIndex = 0; passIndex < passCount; ++passIndex)
		{
			auto* pass = renderGraph->pass(passIndex);
			switch (pass->kind())
			{
			case RenderPassKind::scene_pass: {
				auto* scenePass = static_cast<RenderGraph::ScenePass*>(pass);
				if (scenePass->rtvs().empty() && scenePass->dsv().resource == RenderGraphCompiler::invalid_resource)
				{
					RenderGraphCompiler::DiagnosticItem error;
					error.code = RenderGraphCompiler::DiagnosticCode::error_scene_pass_no_output;
					error.pass = passIndex;
					diagnosticInfo.addError(error);
				}
				break;}
			case RenderPassKind::image_pass: {
				auto* imagePass = static_cast<RenderGraph::ImagePass*>(pass);
				if (!imagePass->pipeline())
				{
					RenderGraphCompiler::DiagnosticItem error;
					error.code = RenderGraphCompiler::DiagnosticCode::error_image_pass_no_pipeline;
					error.pass = passIndex;
					diagnosticInfo.addError(error);
				}
				if (imagePass->rtvs().empty())
				{
					RenderGraphCompiler::DiagnosticItem error;
					error.code = RenderGraphCompiler::DiagnosticCode::error_image_pass_no_rtv;
					error.pass = passIndex;
					diagnosticInfo.addError(error);
				}
				if (imagePass->srvs().empty())
				{
					RenderGraphCompiler::DiagnosticItem warning;
					warning.code = RenderGraphCompiler::DiagnosticCode::warning_image_pass_no_srv;
					warning.pass = passIndex;
					diagnosticInfo.addWarning(warning);
				}
				break;}
			case RenderPassKind::compute_pass: {
				auto* computePass = static_cast<RenderGraph::ComputePass*>(pass);
				if (!computePass->pipeline())
				{
					RenderGraphCompiler::DiagnosticItem error;
					error.code = RenderGraphCompiler::DiagnosticCode::error_compute_pass_no_pipeline;
					error.pass = passIndex;
					diagnosticInfo.addError(error);
				}
				if (computePass->uavs().empty())
				{
					RenderGraphCompiler::DiagnosticItem error;
					error.code = RenderGraphCompiler::DiagnosticCode::error_compute_pass_no_uav;
					error.pass = passIndex;
					diagnosticInfo.addError(error);
				}
				if (computePass->srvs().empty())
				{
					RenderGraphCompiler::DiagnosticItem warning;
					warning.code = RenderGraphCompiler::DiagnosticCode::warning_compute_pass_no_srv;
					warning.pass = passIndex;
					diagnosticInfo.addWarning(warning);
				}
				break;}
			case RenderPassKind::ray_tracing_pass: {
				auto* rayTracingPass = static_cast<RenderGraph::RayTracingPass*>(pass);
				if (!rayTracingPass->pipeline())
				{
					RenderGraphCompiler::DiagnosticItem error;
					error.code = RenderGraphCompiler::DiagnosticCode::error_ray_tracing_pass_no_pipeline;
					error.pass = passIndex;
					diagnosticInfo.addError(error);
				}
				if (rayTracingPass->uavs().empty())
				{
					RenderGraphCompiler::DiagnosticItem error;
					error.code = RenderGraphCompiler::DiagnosticCode::error_ray_tracing_pass_no_uav;
					error.pass = passIndex;
					diagnosticInfo.addError(error);
				}
				if (rayTracingPass->srvs().empty())
				{
					RenderGraphCompiler::DiagnosticItem warning;
					warning.code = RenderGraphCompiler::DiagnosticCode::warning_ray_tracing_pass_no_srv;
					warning.pass = passIndex;
					diagnosticInfo.addWarning(warning);
				}
				break;}
			default:
				GAIA_ASSERT(false && "Unknown pass kind");
				break;
			}
		}
	}

	void CheckPassContribution(
		RenderGraphCompiler::DiagnosticInfo& diagnosticInfo,
		const RenderGraph* renderGraph,
		const std::vector<ResourceUsage>& resourceUsages)
	{
		uint32_t passCount = renderGraph->passCount();
		uint32_t resourceCount = renderGraph->resourceCount();

		std::vector<std::vector<uint32_t>> writersOf(resourceCount);
		std::vector<std::vector<uint32_t>> readsOf(passCount);

		for (uint32_t passIndex = 0; passIndex < passCount; ++passIndex)
		{
			auto* pass = renderGraph->pass(passIndex);
			std::vector<const RenderGraph::ResourceBinding*> bindings;
			pass->collectResourceBindings(bindings);

			for (auto* binding : bindings)
			{
				if (binding->isWrite())
				{
					writersOf[binding->resource].push_back(passIndex);
				}
				if (binding->isRead())
				{
					readsOf[passIndex].push_back(binding->resource);
				}
			}
		}

		std::vector<bool> alivePass(passCount, false);
		std::queue<uint32_t> worklist;

		for (uint32_t outputIndex = 0; outputIndex < renderGraph->outputResourceCount(); ++outputIndex)
		{
			uint32_t resource = renderGraph->outputResource(outputIndex);
			if (resource >= resourceCount)
			{
				continue;
			}
			for (uint32_t writer : writersOf[resource])
			{
				if (!alivePass[writer])
				{
					alivePass[writer] = true;
					worklist.push(writer);
				}
			}
		}

		while (!worklist.empty())
		{
			uint32_t pass = worklist.front();
			worklist.pop();

			for (uint32_t resource : readsOf[pass])
			{
				for (uint32_t writer : writersOf[resource])
				{
					if (!alivePass[writer])
					{
						alivePass[writer] = true;
						worklist.push(writer);
					}
				}
			}
		}
		for (uint32_t passIndex = 0; passIndex < passCount; ++passIndex)
		{
			if (!alivePass[passIndex])
			{
				RenderGraphCompiler::DiagnosticItem warning;
				warning.code = RenderGraphCompiler::DiagnosticCode::warning_pass_no_contribution;
				warning.pass = passIndex;
				diagnosticInfo.addWarning(warning);
			}
		}
	}

	struct BufferDescHash
	{
		size_t operator()(const BufferDesc& desc) const
		{
			uint64_t h = pafcore::fnv1a_64(desc.size);
			h = pafcore::fnv1a_64(desc.bufferUsage, h);
			h = pafcore::fnv1a_64(desc.cpuAccess, h);
			return (size_t)h;
		}
	};

	struct TextureDescHash
	{
		size_t operator()(const TextureDesc& desc) const
		{
			uint64_t h = pafcore::fnv1a_64(desc.width);
			h = pafcore::fnv1a_64(desc.height, h);
			h = pafcore::fnv1a_64(desc.depth, h);
			h = pafcore::fnv1a_64(desc.mipLevels, h);
			h = pafcore::fnv1a_64(desc.arrayLayers, h);
			h = pafcore::fnv1a_64(desc.dimension, h);
			h = pafcore::fnv1a_64(desc.format, h);
			h = pafcore::fnv1a_64(desc.textureUsage, h);
			h = pafcore::fnv1a_64(desc.cpuAccess, h);
			return (size_t)h;
		}
	};

	struct AliasingSlot
	{
		uint32_t firstUsePass = invalid_pass;
		uint32_t lastUsePass = invalid_pass;
		std::string name;
	};

	template<typename Resource_t, typename FreeResourceByDesc_t>
	void MapResourceToSlot(
		std::vector<uint32_t>& resourceToSlot,
		std::vector<AliasingSlot>& slotLifecycles,
		FreeResourceByDesc_t& freeResourceByDesc,
		const ResourceUsage& usage,
		const RenderGraph::Resource* res,
		uint32_t resourceIndex,
		CompiledRenderGraph* compiledRenderGraph)
	{
		const Resource_t* resource = static_cast<const Resource_t*>(res);
		auto it = freeResourceByDesc.find(resource->desc());
		bool allocated = false;
		if (it != freeResourceByDesc.end())
		{
			auto& freeList = it->second;
			for (auto slotIt = freeList.begin(); slotIt != freeList.end(); ++slotIt)
			{
				uint32_t slot = *slotIt;
				GAIA_ASSERT(slot < slotLifecycles.size());
				if (usage.firstUsePass > slotLifecycles[slot].lastUsePass)
				{
					resourceToSlot[resourceIndex] = slot;
					slotLifecycles[slot].firstUsePass = std::min(slotLifecycles[slot].firstUsePass, usage.firstUsePass);
					slotLifecycles[slot].lastUsePass = std::max(slotLifecycles[slot].lastUsePass, usage.lastUsePass);
					slotLifecycles[slot].name += "," + resource->name();
					freeList.erase(slotIt);
					allocated = true;
					break;
				}
			}
		}
		if (!allocated)
		{
			uint32_t slot = compiledRenderGraph->addResource(resource->name(), resource->desc());
			GAIA_ASSERT(slot == slotLifecycles.size());
			resourceToSlot[resourceIndex] = slot;
			slotLifecycles.push_back({ usage.firstUsePass, usage.lastUsePass, resource->name() });
		}
	}

	std::vector<uint32_t> BuildCompiledResources(
		CompiledRenderGraph* compiledRenderGraph,
		const RenderGraph* renderGraph,
		const std::vector<ResourceUsage>& resourceUsages)
	{
		uint32_t passCount = renderGraph->passCount();
		uint32_t resourceCount = renderGraph->resourceCount();
		std::vector<uint32_t> resourceToSlot(resourceCount, invalid_resource);
		std::vector<AliasingSlot> slotLifecycles;
		std::unordered_map<TextureDesc, std::vector<uint32_t>, TextureDescHash> freeTextureByDesc;
		std::unordered_map<BufferDesc, std::vector<uint32_t>, BufferDescHash> freeBufferByDesc;

		std::vector<std::vector<uint32_t>> resourcesStartingAt(passCount);
		std::vector<std::vector<uint32_t>> resourcesEndingAt(passCount);
		for (uint32_t resourceIndex = 0; resourceIndex < resourceCount; ++resourceIndex)
		{
			const auto& usage = resourceUsages[resourceIndex];
			if (usage.firstUsePass != invalid_pass)
			{
				resourcesStartingAt[usage.firstUsePass].push_back(resourceIndex);
			}
			if (usage.lastUsePass != invalid_pass)
			{
				resourcesEndingAt[usage.lastUsePass].push_back(resourceIndex);
			}
		}

		for (uint32_t passIndex = 0; passIndex < passCount; ++passIndex)
		{
			for (uint32_t resourceIndex : resourcesStartingAt[passIndex])
			{
				const auto& usage = resourceUsages[resourceIndex];
				GAIA_ASSERT(usage.firstUsePass == passIndex);
				if (resourceToSlot[resourceIndex] != invalid_resource)
				{
					continue;
				}
				auto* resource = renderGraph->resource(resourceIndex);
				if (resource->isExternal())
				{
					uint32_t slot;
					if (resource->kind() == RenderGraph::ResourceKind::external_texture)
					{
						slot = compiledRenderGraph->cloneExternalTexture(static_cast<RenderGraph::ExternalTexture*>(resource));
					}
					else
					{
						GAIA_ASSERT(resource->kind() == RenderGraph::ResourceKind::external_buffer);
						slot = compiledRenderGraph->cloneExternalBuffer(static_cast<RenderGraph::ExternalBuffer*>(resource));
					}
					GAIA_ASSERT(slot == slotLifecycles.size());
					resourceToSlot[resourceIndex] = slot;
					slotLifecycles.push_back({ passIndex, usage.lastUsePass, resource->name() });
					continue;
				}

				if (resource->kind() == RenderGraph::ResourceKind::texture)
				{
					MapResourceToSlot<RenderGraph::Texture>(resourceToSlot, slotLifecycles, freeTextureByDesc, usage, resource, resourceIndex, compiledRenderGraph);
				}
				else if (resource->kind() == RenderGraph::ResourceKind::buffer)
				{
					MapResourceToSlot<RenderGraph::Buffer>(resourceToSlot, slotLifecycles, freeBufferByDesc, usage, resource, resourceIndex, compiledRenderGraph);
				}
			}
			for (uint32_t resourceIndex : resourcesEndingAt[passIndex])
			{
				if (resourceToSlot[resourceIndex] == invalid_resource)
				{
					continue;
				}
				uint32_t slot = resourceToSlot[resourceIndex];
				GAIA_ASSERT(slot < slotLifecycles.size());
				auto* resource = renderGraph->resource(resourceIndex);
				compiledRenderGraph->resource(slot)->name(slotLifecycles[slot].name);

				if (resource->kind() == RenderGraph::ResourceKind::texture)
				{
					auto* tex = static_cast<const RenderGraph::Texture*>(resource);
					freeTextureByDesc[tex->desc()].push_back(slot);
				}
				else if (resource->kind() == RenderGraph::ResourceKind::buffer)
				{
					auto* buf = static_cast<const RenderGraph::Buffer*>(resource);
					freeBufferByDesc[buf->desc()].push_back(slot);
				}
			}
		}
		return resourceToSlot;
	}


	void BuildCompiledPasses(
		RenderGraphCompiler::DiagnosticInfo& diagnosticInfo,
		CompiledRenderGraph* compiledRenderGraph,
		const RenderGraph* renderGraph,
		const std::vector<uint32_t>& resourceToSlot)
	{
		uint32_t passCount = renderGraph->passCount();

		for (uint32_t passIndex = 0; passIndex < passCount; ++passIndex)
		{
			auto* srcPass = renderGraph->pass(passIndex);
			auto* dstPass = compiledRenderGraph->addPassFromSource(srcPass);
			dstPass->buildResourceBindings(srcPass, resourceToSlot);
		}

		uint32_t resourceCount = compiledRenderGraph->resourceCount();

		std::vector<const CompiledRenderGraph::ResourceBinding*> lastBinding(resourceCount, nullptr);

		for (uint32_t passIndex = 0; passIndex < passCount; ++passIndex)
		{
			auto* pass = compiledRenderGraph->pass(passIndex);
			std::vector<const CompiledRenderGraph::ResourceBinding*> bindings;
			pass->collectResourceBindings(bindings);

			for (auto* binding : bindings)
			{
				uint32_t resource = binding->resource;
				GAIA_ASSERT(resource < resourceCount);
				pass->addTransition(lastBinding[resource], binding);
				lastBinding[resource] = binding;
			}
		}

		for (uint32_t passIndex = 0; passIndex < passCount; ++passIndex)
		{
			auto* pass = compiledRenderGraph->pass(passIndex);
			if (pass->kind() == RenderPassKind::scene_pass)
			{
				auto* scenePass = static_cast<CompiledRenderGraph::ScenePass*>(pass);
				if(scenePass->rtvs().empty() && scenePass->dsv().resource == invalid_resource)
				{
					RenderGraphCompiler::DiagnosticItem error;
					error.code = RenderGraphCompiler::DiagnosticCode::error_scene_pass_no_output;
					error.resource = invalid_resource;
					error.pass = passIndex;
					diagnosticInfo.addError(error);
				}
			}
			else if (pass->kind() == RenderPassKind::image_pass)
			{
				auto* imagePass = static_cast<CompiledRenderGraph::ImagePass*>(pass);
				if (imagePass->rtvs().empty())
				{
					RenderGraphCompiler::DiagnosticItem error;
					error.code = RenderGraphCompiler::DiagnosticCode::error_image_pass_no_rtv;
					error.resource = invalid_resource;
					error.pass = passIndex;
					diagnosticInfo.addError(error);
				}
				if (imagePass->srvs().empty())
				{
					RenderGraphCompiler::DiagnosticItem warning;
					warning.code = RenderGraphCompiler::DiagnosticCode::warning_image_pass_no_srv;
					warning.resource = invalid_resource;
					warning.pass = passIndex;
					diagnosticInfo.addWarning(warning);
				}
			}
			else if (pass->kind() == RenderPassKind::compute_pass)
			{
				auto* computePass = static_cast<CompiledRenderGraph::ComputePass*>(pass);
				if (computePass->uavs().empty())
				{
					RenderGraphCompiler::DiagnosticItem error;
					error.code = RenderGraphCompiler::DiagnosticCode::error_compute_pass_no_uav;
					error.resource = invalid_resource;
					error.pass = passIndex;
					diagnosticInfo.addError(error);
				}
				if (computePass->srvs().empty())
				{
					RenderGraphCompiler::DiagnosticItem warning;
					warning.code = RenderGraphCompiler::DiagnosticCode::warning_compute_pass_no_srv;
					warning.resource = invalid_resource;
					warning.pass = passIndex;
					diagnosticInfo.addWarning(warning);
				}
			}
		}
	}
}


RenderGraphCompiler::DiagnosticInfo RenderGraphCompiler::compile(CompiledRenderGraph* compiledRenderGraph, const RenderGraph* renderGraph)
{
	RenderGraphCompiler::DiagnosticInfo diagnosticInfo;

	compiledRenderGraph->reset();

	std::vector<ResourceUsage> resourceUsages = CheckResources(diagnosticInfo, renderGraph);
	CheckOutputResources(diagnosticInfo, renderGraph, resourceUsages);
	CheckPasses(diagnosticInfo, renderGraph);
	if (!diagnosticInfo.errors.empty())
	{
		return diagnosticInfo;
	}

	std::vector<uint32_t> resourceToSlot = BuildCompiledResources(compiledRenderGraph, renderGraph, resourceUsages);

	BuildCompiledPasses(diagnosticInfo, compiledRenderGraph, renderGraph, resourceToSlot);
	CheckPassContribution(diagnosticInfo, renderGraph, resourceUsages);

	return diagnosticInfo;
}

END_GAIA
