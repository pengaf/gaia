#include "render_graph_translator.h"
#include "render_system.h"
#include "resource.h"
#include "pipeline.h"
#include "../compiled_render_graph.h"
#include "detail/mapping.h"
#include "pafcore/hash.h"
#include <unordered_map>
#include <vector>

BEGIN_GAIA_VK1

namespace
{
	constexpr uint32_t invalid_pass = UINT32_MAX;
	constexpr uint32_t invalid_resource = CompiledRenderGraph::invalid_resource;
	constexpr uint32_t invalid_resource_view = UINT32_MAX;

	struct ResourceMap
	{
		std::unordered_map<uint32_t, uint32_t> textures;
		std::unordered_map<uint32_t, uint32_t> buffers;
		std::unordered_map<uint32_t, uint32_t> externalTextures;
		std::unordered_map<uint32_t, uint32_t> externalBuffers;
		std::vector<TextureDesc> textureDescs;
		std::vector<BufferDesc> bufferDescs;
	};

	struct RtvKey
	{
		VkImage image;
		RenderTargetViewDesc desc;
		bool operator==(const RtvKey& other) const = default;
	};

	struct RtvKeyHash
	{
		size_t operator()(const RtvKey& key) const
		{
			uint64_t h = pafcore::fnv1a_64(key.image);
			h = pafcore::fnv1a_64(key.desc.dimension, h);
			h = pafcore::fnv1a_64(key.desc.format, h);
			h = pafcore::fnv1a_64(key.desc.mipLevel, h);
			h = pafcore::fnv1a_64(key.desc.baseArrayLayer, h);
			h = pafcore::fnv1a_64(key.desc.layerCount, h);
			h = pafcore::fnv1a_64(key.desc.clear, h);
			h = pafcore::fnv1a_64(key.desc.clearColor.r, h);
			h = pafcore::fnv1a_64(key.desc.clearColor.g, h);
			h = pafcore::fnv1a_64(key.desc.clearColor.b, h);
			h = pafcore::fnv1a_64(key.desc.clearColor.a, h);
			return (size_t)h;
		}
	};

	struct DsvKey
	{
		VkImage image;
		DepthStencilViewDesc desc;
		bool operator==(const DsvKey& other) const = default;
	};

	struct DsvKeyHash
	{
		size_t operator()(const DsvKey& key) const
		{
			uint64_t h = pafcore::fnv1a_64(key.image);
			h = pafcore::fnv1a_64(key.desc.dimension, h);
			h = pafcore::fnv1a_64(key.desc.format, h);
			h = pafcore::fnv1a_64(key.desc.mipLevel, h);
			h = pafcore::fnv1a_64(key.desc.baseArrayLayer, h);
			h = pafcore::fnv1a_64(key.desc.layerCount, h);
			h = pafcore::fnv1a_64(key.desc.readOnly, h);
			h = pafcore::fnv1a_64(key.desc.clearDepth, h);
			h = pafcore::fnv1a_64(key.desc.clearStencil, h);
			h = pafcore::fnv1a_64(key.desc.clearStencilValue, h);
			h = pafcore::fnv1a_64(key.desc.clearDepthValue, h);
			return (size_t)h;
		}
	};

	struct TextureSrvUavKey
	{
		VkImage image;
		TextureViewDesc desc;
		auto operator<=>(const TextureSrvUavKey& other) const = default;
	};

	struct TextureSrvUavKeyHash
	{
		size_t operator()(const TextureSrvUavKey& key) const
		{
			uint64_t h = pafcore::fnv1a_64(key.image);
			h = pafcore::fnv1a_64(key.desc.dimension, h);
			h = pafcore::fnv1a_64(key.desc.format, h);
			h = pafcore::fnv1a_64(key.desc.baseMipLevel, h);
			h = pafcore::fnv1a_64(key.desc.levelCount, h);
			h = pafcore::fnv1a_64(key.desc.baseArrayLayer, h);
			h = pafcore::fnv1a_64(key.desc.layerCount, h);
			return (size_t)h;
		}
	};

	struct TexelBufferSrvUavKey
	{
		VkBuffer buffer;
		TexelBufferViewDesc desc;
		auto operator<=>(const TexelBufferSrvUavKey& other) const = default;
	};

	struct TexelBufferSrvUavKeyHash
	{
		size_t operator()(const TexelBufferSrvUavKey& key) const
		{
			uint64_t h = pafcore::fnv1a_64(key.buffer);
			h = pafcore::fnv1a_64(key.desc.offset, h);
			h = pafcore::fnv1a_64(key.desc.size, h);
			h = pafcore::fnv1a_64(key.desc.format, h);
			return (size_t)h;
		}
	};

	struct RtvInfo
	{
		uint32_t viewIndex = invalid_resource_view;
		uint32_t width = UINT32_MAX;
		uint32_t height = UINT32_MAX;
	};

	struct DsvInfo
	{
		uint32_t viewIndex = invalid_resource_view;
		uint32_t width = UINT32_MAX;
		uint32_t height = UINT32_MAX;
	};

	struct ResourceViewMap
	{
		std::unordered_map<RtvKey, RtvInfo, RtvKeyHash> rtvMap;
		std::unordered_map<DsvKey, DsvInfo, DsvKeyHash> dsvMap;
		std::unordered_map<TextureSrvUavKey, uint32_t, TextureSrvUavKeyHash> textureSrvUavMap;
		std::unordered_map<TexelBufferSrvUavKey, uint32_t, TexelBufferSrvUavKeyHash> texelBufferSrvMap;
	};

	ResourceMap BuildResources(
		RenderGraphTranslator::DiagnosticInfo& diagnosticInfo,
		ExecutableRenderGraph* executableRenderGraph,
		const CompiledRenderGraph* compiledRenderGraph,
		RenderSystem* renderSystem)
	{
		ResourceMap resourceMap;
		uint32_t resourceCount = compiledRenderGraph->resourceCount();
		for (uint32_t resourceIndex = 0; resourceIndex < resourceCount; ++resourceIndex)
		{
			CompiledRenderGraph::Resource* resource = compiledRenderGraph->resource(resourceIndex);
			switch (resource->kind())
			{
			case CompiledRenderGraph::ResourceKind::texture: {
				CompiledRenderGraph::Texture* texture = static_cast<CompiledRenderGraph::Texture*>(resource);
				TextureHandle textureHandle = renderSystem->createTextureHandle(texture->desc());
				if (textureHandle.isValid())
				{
					resourceMap.textures[resourceIndex] = executableRenderGraph->m_textures.size();
					executableRenderGraph->m_textures.push_back(textureHandle);
					resourceMap.textureDescs.push_back(texture->desc());
					GAIA_ASSERT(executableRenderGraph->m_textures.size() == resourceMap.textureDescs.size());
				}
				else
				{
					RenderGraphTranslator::DiagnosticItem error;
					error.code = RenderGraphTranslator::DiagnosticCode::error_create_texture_failed;
					error.resource = resourceIndex;
					diagnosticInfo.addError(error);
				}
				break;
			}
			case CompiledRenderGraph::ResourceKind::buffer:
			{
				CompiledRenderGraph::Buffer* buffer = static_cast<CompiledRenderGraph::Buffer*>(resource);
				BufferHandle bufferHandle = renderSystem->createBufferHandle(buffer->desc());
				if (bufferHandle.isValid())
				{
					resourceMap.buffers[resourceIndex] = executableRenderGraph->m_buffers.size();
					executableRenderGraph->m_buffers.push_back(bufferHandle);
					resourceMap.bufferDescs.push_back(buffer->desc());
					GAIA_ASSERT(executableRenderGraph->m_buffers.size() == resourceMap.bufferDescs.size());
				}
				else
				{
					RenderGraphTranslator::DiagnosticItem error;
					error.code = RenderGraphTranslator::DiagnosticCode::error_create_buffer_failed;
					error.resource = resourceIndex;
					diagnosticInfo.addError(error);
				}
			}
			break;
			case CompiledRenderGraph::ResourceKind::external_texture:
			{
				CompiledRenderGraph::ExternalTexture* externalTexture = static_cast<CompiledRenderGraph::ExternalTexture*>(resource);
				Texture* texture = externalTexture->texture();
				GAIA_ASSERT(texture != nullptr);
				resourceMap.externalTextures[resourceIndex] = executableRenderGraph->m_externalTextures.size();
				executableRenderGraph->m_externalTextures.push_back(RefPtr<Texture>(texture));
			}
			break;
			case CompiledRenderGraph::ResourceKind::external_buffer:
			{
				CompiledRenderGraph::ExternalBuffer* externalBuffer = static_cast<CompiledRenderGraph::ExternalBuffer*>(resource);
				Buffer* buffer = externalBuffer->buffer();
				GAIA_ASSERT(buffer != nullptr);
				resourceMap.externalBuffers[resourceIndex] = executableRenderGraph->m_externalBuffers.size();
				executableRenderGraph->m_externalBuffers.push_back(RefPtr<Buffer>(buffer));
			}
			default:
				GAIA_ASSERT(false && "Unknown resource kind");
				break;
			}
		}
		return resourceMap;
	}

	struct TextureInfo
	{
		VkImage image = VK_NULL_HANDLE;
		const TextureDesc* desc = nullptr;
	};

	TextureInfo GetImageFromResourceIndex(
		RenderGraphTranslator::DiagnosticInfo& diagnosticInfo,
		uint32_t passIndex,
		const ResourceMap& resourceMap,
		ExecutableRenderGraph* dstRenderGraph,
		const CompiledRenderGraph* srcRenderGraph,
		uint32_t resourceIndex)
	{
		TextureInfo textureInfo;
		const CompiledRenderGraph::Resource* srcResource = srcRenderGraph->resource(resourceIndex);
		switch (srcResource->kind())
		{
		case CompiledRenderGraph::ResourceKind::texture: {
			auto it = resourceMap.textures.find(resourceIndex);
			GAIA_ASSERT(resourceMap.textures.end() != it);
			GAIA_ASSERT(it->second < dstRenderGraph->m_textures.size());
			textureInfo.image = dstRenderGraph->m_textures[it->second].image();
			textureInfo.desc = &resourceMap.textureDescs[it->second];
			break;
		}
		case CompiledRenderGraph::ResourceKind::external_texture: {
			auto it = resourceMap.externalTextures.find(resourceIndex);
			GAIA_ASSERT(resourceMap.externalTextures.end() != it);
			GAIA_ASSERT(it->second < dstRenderGraph->m_externalTextures.size());
			Texture* externalTexture = dstRenderGraph->m_externalTextures[it->second].get();
			textureInfo.image = externalTexture->nativeHandle();
			textureInfo.desc = &externalTexture->desc();
			break;
		}
		default: {
			RenderGraphTranslator::DiagnosticItem error;
			error.code = RenderGraphTranslator::DiagnosticCode::error_resource_is_not_texture;
			error.pass = passIndex;
			error.resource = resourceIndex;
			diagnosticInfo.addError(error);
			return textureInfo;
		}
		}
		if (VK_NULL_HANDLE == textureInfo.image)
		{
			RenderGraphTranslator::DiagnosticItem error;
			error.code = RenderGraphTranslator::DiagnosticCode::error_invalid_texture;
			error.pass = passIndex;
			error.resource = resourceIndex;
			diagnosticInfo.addError(error);
		}
		return textureInfo;
	}


	RtvInfo GetRenderTargetView(
		RenderGraphTranslator::DiagnosticInfo& diagnosticInfo,
		uint32_t passIndex,
		const ResourceMap& resourceMap,
		ResourceViewMap& resourceViewMap,
		ExecutableRenderGraph* dstRenderGraph,
		const CompiledRenderGraph* srcRenderGraph,
		const CompiledRenderGraph::RTV& rtv,
		RenderSystem* renderSystem)
	{
		RtvInfo rtvInfo;
		TextureInfo textureInfo = GetImageFromResourceIndex(diagnosticInfo, passIndex, resourceMap, dstRenderGraph, srcRenderGraph, rtv.resource);
		if (VK_NULL_HANDLE == textureInfo.image)
		{
			return rtvInfo;
		}
		RtvKey rtvKey = { textureInfo.image, rtv.usageDesc };
		auto it = resourceViewMap.rtvMap.find(rtvKey);
		if (resourceViewMap.rtvMap.end() == it)
		{
			VkDevice device = renderSystem->device();
			VkImageViewCreateInfo createInfo;
			MapRenderTargetViewDesc(createInfo, rtv.usageDesc, textureInfo.image);
			if (createInfo.subresourceRange.aspectMask != VK_IMAGE_ASPECT_COLOR_BIT)
			{
				RenderGraphTranslator::DiagnosticItem error;
				error.code = RenderGraphTranslator::DiagnosticCode::error_invalid_rtv_format;
				error.pass = passIndex;
				error.resource = rtv.resource;
				diagnosticInfo.addError(error);
				return rtvInfo;
			}
			VkImageView imageView;
			VkResult result = vkCreateImageView(device, &createInfo, nullptr, &imageView);
			if (VK_SUCCESS != result)
			{
				RenderGraphTranslator::DiagnosticItem error;
				error.code = RenderGraphTranslator::DiagnosticCode::error_create_image_view_failed;
				error.pass = passIndex;
				error.resource = rtv.resource;
				diagnosticInfo.addError(error);
				return rtvInfo;
			}
			uint32_t viewIndex = dstRenderGraph->m_imageViews.size();
			dstRenderGraph->m_imageViews.push_back(imageView);
			rtvInfo.viewIndex = viewIndex;
			rtvInfo.width = std::max(1u, textureInfo.desc->width >> rtv.usageDesc.mipLevel);
			rtvInfo.height = std::max(1u, textureInfo.desc->height >> rtv.usageDesc.mipLevel);
			resourceViewMap.rtvMap.insert(std::make_pair(rtvKey, rtvInfo));
			return rtvInfo;
		}
		else
		{
			return it->second;
		}
	}

	DsvInfo GetDepthStencilView(
		RenderGraphTranslator::DiagnosticInfo& diagnosticInfo,
		uint32_t passIndex,
		const ResourceMap& resourceMap,
		ResourceViewMap& resourceViewMap,
		ExecutableRenderGraph* dstRenderGraph,
		const CompiledRenderGraph* srcRenderGraph,
		const CompiledRenderGraph::DSV& dsv,
		RenderSystem* renderSystem)
	{
		DsvInfo dsvInfo;
		TextureInfo textureInfo = GetImageFromResourceIndex(diagnosticInfo, passIndex, resourceMap, dstRenderGraph, srcRenderGraph, dsv.resource);
		if (VK_NULL_HANDLE == textureInfo.image)
		{
			return dsvInfo;
		}

		DsvKey dsvKey = { textureInfo.image, dsv.usageDesc };
		auto it = resourceViewMap.dsvMap.find(dsvKey);
		if (resourceViewMap.dsvMap.end() == it)
		{
			VkDevice device = renderSystem->device();
			VkImageViewCreateInfo createInfo;
			MapDepthStencilViewDesc(createInfo, dsv.usageDesc, textureInfo.image);
			if ((createInfo.subresourceRange.aspectMask & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) == 0)
			{
				RenderGraphTranslator::DiagnosticItem error;
				error.code = RenderGraphTranslator::DiagnosticCode::error_invalid_dsv_format;
				error.pass = passIndex;
				error.resource = dsv.resource;
				diagnosticInfo.addError(error);
				return dsvInfo;
			}
			VkImageView imageView;
			VkResult result = vkCreateImageView(device, &createInfo, nullptr, &imageView);
			if (VK_SUCCESS != result)
			{
				RenderGraphTranslator::DiagnosticItem error;
				error.code = RenderGraphTranslator::DiagnosticCode::error_create_image_view_failed;
				error.pass = passIndex;
				error.resource = dsv.resource;
				diagnosticInfo.addError(error);
				return dsvInfo;
			}
			uint32_t viewIndex = dstRenderGraph->m_imageViews.size();
			dstRenderGraph->m_imageViews.push_back(imageView);
			dsvInfo.viewIndex = viewIndex;
			dsvInfo.width = std::max(1u, textureInfo.desc->width >> dsv.usageDesc.mipLevel);
			dsvInfo.height = std::max(1u, textureInfo.desc->height >> dsv.usageDesc.mipLevel);
			resourceViewMap.dsvMap.insert(std::make_pair(dsvKey, dsvInfo));
			return dsvInfo;
		}
		else
		{
			return it->second;
		}
	}


	void BuildScenePass(
		RenderGraphTranslator::DiagnosticInfo& diagnosticInfo,
		uint32_t passIndex,
		ExecutableRenderGraph::ScenePass* dstScenePass,
		const CompiledRenderGraph::ScenePass* srcScenePass,
		ExecutableRenderGraph* dstRenderGraph,
		const CompiledRenderGraph* srcRenderGraph,
		const ResourceMap& resourceMap,
		ResourceViewMap& resourceViewMap,
		RenderSystem* renderSystem)
	{
		uint32_t renderWidth = UINT32_MAX;
		uint32_t renderHeight = UINT32_MAX;

		GAIA_ASSERT(srcScenePass->rtvs().size() <= gaia_max_color_attment_count);
		uint32_t rtvCount = srcScenePass->rtvs().size();
		if(gaia_max_color_attment_count < rtvCount)
		{
			RenderGraphTranslator::DiagnosticItem error;
			error.code = RenderGraphTranslator::DiagnosticCode::error_scene_pass_too_many_rtvs;
			error.pass = passIndex;
			diagnosticInfo.addError(error);
			//continue for checking other errors
			rtvCount = gaia_max_color_attment_count;
		}

		dstScenePass->m_beginRendering.m_renderingInfo.colorAttachmentCount = rtvCount;
		for (uint32_t i = 0; i < rtvCount; ++i)
		{
			auto& rtv = srcScenePass->rtvs()[i];
			RtvInfo rtvInfo = GetRenderTargetView(diagnosticInfo, passIndex, resourceMap, resourceViewMap, dstRenderGraph, srcRenderGraph, rtv, renderSystem);
			if (invalid_resource_view != rtvInfo.viewIndex)
			{
				auto& colorAttachmentInfo = dstScenePass->m_beginRendering.m_colorAttachmentInfos[i];
				colorAttachmentInfo.imageView = dstRenderGraph->m_imageViews[rtvInfo.viewIndex];
				renderWidth = std::min(renderWidth, rtvInfo.width);
				renderHeight = std::min(renderHeight, rtvInfo.height);
			}
		}
		auto& dsv = srcScenePass->dsv();
		if (dsv.resource != invalid_resource)
		{
			DsvInfo dsvInfo = GetDepthStencilView(diagnosticInfo, passIndex, resourceMap, resourceViewMap, dstRenderGraph, srcRenderGraph, dsv, renderSystem);
			if (invalid_resource_view != dsvInfo.viewIndex)
			{
				dstScenePass->m_beginRendering.m_depthStencilAttachmentInfo.imageView = dstRenderGraph->m_imageViews[dsvInfo.viewIndex];
				VkImageAspectFlags aspectMask = MapTextureAspect(dsv.usageDesc.format);
				if (0 == (aspectMask & VK_IMAGE_ASPECT_DEPTH_BIT))
				{
					dstScenePass->m_beginRendering.m_renderingInfo.pDepthAttachment = nullptr;
				}
				if (0 == (aspectMask & VK_IMAGE_ASPECT_STENCIL_BIT))
				{
					dstScenePass->m_beginRendering.m_renderingInfo.pStencilAttachment = nullptr;
				}
				renderWidth = std::min(renderWidth, dsvInfo.width);
				renderHeight = std::min(renderHeight, dsvInfo.height);
			}
		}
		dstScenePass->m_beginRendering.m_renderingInfo.renderArea.extent.width = renderWidth;
		dstScenePass->m_beginRendering.m_renderingInfo.renderArea.extent.height = renderHeight;

		if (0 == rtvCount && invalid_resource == dsv.resource)
		{
			RenderGraphTranslator::DiagnosticItem error;
			error.code = RenderGraphTranslator::DiagnosticCode::error_scene_pass_no_output;
			error.pass = passIndex;
			diagnosticInfo.addError(error);
		}
		if (UINT32_MAX == renderWidth || 0 == renderWidth || UINT32_MAX == renderHeight || 0 == renderHeight)
		{
			RenderGraphTranslator::DiagnosticItem error;
			error.code = RenderGraphTranslator::DiagnosticCode::error_scene_pass_invalid_render_size;
			error.pass = passIndex;
			diagnosticInfo.addError(error);
		}
	}


	void BuildImagePass(
		RenderGraphTranslator::DiagnosticInfo& diagnosticInfo,
		uint32_t passIndex,
		ExecutableRenderGraph::ImagePass* dstImagePass,
		const CompiledRenderGraph::ImagePass* srcImagePass,
		ExecutableRenderGraph* dstRenderGraph,
		const CompiledRenderGraph* srcRenderGraph,
		const ResourceMap& resourceMap,
		ResourceViewMap& resourceViewMap,
		RenderSystem* renderSystem)
	{
		uint32_t renderWidth = UINT32_MAX;
		uint32_t renderHeight = UINT32_MAX;

		GAIA_ASSERT(srcImagePass->rtvs().size() <= gaia_max_color_attment_count);
		uint32_t rtvCount = srcImagePass->rtvs().size();
		if (gaia_max_color_attment_count < rtvCount)
		{
			RenderGraphTranslator::DiagnosticItem error;
			error.code = RenderGraphTranslator::DiagnosticCode::error_image_pass_too_many_rtvs;
			error.pass = passIndex;
			diagnosticInfo.addError(error);
			//continue for checking other errors
			rtvCount = gaia_max_color_attment_count;
		}

		dstImagePass->m_beginRendering.m_renderingInfo.colorAttachmentCount = rtvCount;
		for (uint32_t i = 0; i < rtvCount; ++i)
		{
			auto& rtv = srcImagePass->rtvs()[i];
			RtvInfo rtvInfo = GetRenderTargetView(diagnosticInfo, passIndex, resourceMap, resourceViewMap, dstRenderGraph, srcRenderGraph, rtv, renderSystem);
			if (invalid_resource_view != rtvInfo.viewIndex)
			{
				auto& colorAttachmentInfo = dstImagePass->m_beginRendering.m_colorAttachmentInfos[i];
				colorAttachmentInfo.imageView = dstRenderGraph->m_imageViews[rtvInfo.viewIndex];
				renderWidth = std::min(renderWidth, rtvInfo.width);
				renderHeight = std::min(renderHeight, rtvInfo.height);
			}
		}
		dstImagePass->m_beginRendering.m_renderingInfo.renderArea.extent.width = renderWidth;
		dstImagePass->m_beginRendering.m_renderingInfo.renderArea.extent.height = renderHeight;
		if (0 == rtvCount)
		{
			RenderGraphTranslator::DiagnosticItem error;
			error.code = RenderGraphTranslator::DiagnosticCode::error_image_pass_no_output;
			error.pass = passIndex;
			diagnosticInfo.addError(error);
		}
		if (UINT32_MAX == renderWidth || 0 == renderWidth || UINT32_MAX == renderHeight || 0 == renderHeight)
		{
			RenderGraphTranslator::DiagnosticItem error;
			error.code = RenderGraphTranslator::DiagnosticCode::error_image_pass_invalid_render_size;
			error.pass = passIndex;
			diagnosticInfo.addError(error);
		}

		const GraphicsPipeline* graphicsPipeline = srcImagePass->pipeline();
		//VkPipeline pipeline = graphicsPipeline->getPipeline();
		VkPipelineLayout pipelineLayout = graphicsPipeline->getPipelineLayout();
		if (VK_NULL_HANDLE == pipelineLayout)
		{
			RenderGraphTranslator::DiagnosticItem error;
			error.code = RenderGraphTranslator::DiagnosticCode::error_invalid_graphics_pipeline;
			error.pass = passIndex;
			diagnosticInfo.addError(error);
		}
		//dstImagePass->m_bindPipeline.m_pipeline = pipeline;
		dstImagePass->m_bindDescriptorSets.m_pipelineLayout = pipelineLayout;
		dstImagePass->m_draw.initialize(srcImagePass->vertexCount());
	}


	void BuildComputePass(
		RenderGraphTranslator::DiagnosticInfo& diagnosticInfo,
		uint32_t passIndex,
		ExecutableRenderGraph::ComputePass* dstComputePass,
		const CompiledRenderGraph::ComputePass* srcComputePass,
		ExecutableRenderGraph* dstRenderGraph,
		const CompiledRenderGraph* srcRenderGraph,
		const ResourceMap& resourceMap,
		ResourceViewMap& resourceViewMap,
		RenderSystem* renderSystem)
	{
		//GAIA_ASSERT(srcComputePass->uavs().size() <= gaia_max_uav_count);
		uint32_t uavCount = srcComputePass->uavs().size();
		const ComputePipeline* computePipeline = srcComputePass->pipeline();
		VkPipeline pipeline = computePipeline->getPipeline();
		VkPipelineLayout pipelineLayout = computePipeline->getPipelineLayout();
		if (VK_NULL_HANDLE == pipeline || VK_NULL_HANDLE == pipelineLayout)
		{
			RenderGraphTranslator::DiagnosticItem error;
			error.code = RenderGraphTranslator::DiagnosticCode::error_invalid_compute_pipeline;
			error.pass = passIndex;
			diagnosticInfo.addError(error);
		}
		dstComputePass->m_bindPipeline.m_pipeline = pipeline;
		dstComputePass->m_bindDescriptorSets.m_pipelineLayout = pipelineLayout;	
		dstComputePass->m_dispatch.initialize(srcComputePass->dispatchX(), srcComputePass->dispatchY(), srcComputePass->dispatchZ());
	}

	void BuildPasses(
		RenderGraphTranslator::DiagnosticInfo& diagnosticInfo,
		ExecutableRenderGraph* dstRenderGraph,
		const CompiledRenderGraph* srcRenderGraph,
		const ResourceMap& resourceMap,
		ResourceViewMap& resourceViewMap,
		RenderSystem* renderSystem)
	{
		uint32_t passCount = srcRenderGraph->passCount();
		for (uint32_t passIndex = 0; passIndex < passCount; ++passIndex)
		{
			const CompiledRenderGraph::Pass* srcPass = srcRenderGraph->pass(passIndex);
			switch (srcPass->kind())
			{
			case RenderPassKind::scene_pass:
			{
				auto* dstScenePass = dstRenderGraph->addScenePass(srcPass->name());
				BuildScenePass(diagnosticInfo, passIndex, dstScenePass, static_cast<const CompiledRenderGraph::ScenePass*>(srcPass),
					dstRenderGraph, srcRenderGraph, resourceMap, resourceViewMap, renderSystem);
			}
			break;
			case RenderPassKind::image_pass: 
			{
				auto* dstImagePass = dstRenderGraph->addImagePass(srcPass->name());
				BuildImagePass(diagnosticInfo, passIndex, dstImagePass, static_cast<const CompiledRenderGraph::ImagePass*>(srcPass),
					dstRenderGraph, srcRenderGraph, resourceMap, resourceViewMap, renderSystem);
			}
			break;
			case RenderPassKind::compute_pass: 
			{
				auto* dstComputePass = dstRenderGraph->addComputePass(srcPass->name());
				BuildComputePass(diagnosticInfo, passIndex, dstComputePass, static_cast<const CompiledRenderGraph::ComputePass*>(srcPass),
					dstRenderGraph, srcRenderGraph, resourceMap, resourceViewMap, renderSystem);
			}
			break;
			}
		}
	}
}


RenderGraphTranslator::DiagnosticInfo RenderGraphTranslator::translate(ExecutableRenderGraph* executableRenderGraph, const CompiledRenderGraph* compiledRenderGraph, RenderSystem* renderSystem)
{
	RenderGraphTranslator::DiagnosticInfo diagnosticInfo;
	executableRenderGraph->reset(renderSystem);
	ResourceMap resourceMap = BuildResources(diagnosticInfo, executableRenderGraph, compiledRenderGraph, renderSystem);
	if (!diagnosticInfo.errors.empty())
	{
		return diagnosticInfo;
	}
	ResourceViewMap resourceViewMap;

	return diagnosticInfo;
}

END_GAIA_VK1
