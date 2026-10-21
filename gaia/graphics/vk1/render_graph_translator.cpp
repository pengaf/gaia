#include "render_graph_translator.h"
#include "render_system.h"
#include "../compiled_render_graph.h"
#include "detail/mapping.h"
#include <unordered_map>

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
	};

	struct RtvKey
	{
		VkImage image;
		RenderTargetViewDesc desc;
	};

	struct DsvKey
	{
		VkImage image;
		DepthStencilViewDesc desc;
	};

	struct TextureSrvUavKey
	{
		VkImage image;
		TextureViewDesc desc;
	};

	//struct BufferSrvUavKey
	//{
	//	VkBuffer buffer;
	//	BufferViewDesc desc;
	//};

	struct TexelBufferSrvUavKey
	{
		VkBuffer buffer;
		TexelBufferViewDesc desc;
	};

	struct ResourceViewMap
	{
		std::unordered_map<RtvKey, uint32_t> rtvMap;
		std::unordered_map<DsvKey, uint32_t> dsvMap;
		std::unordered_map<TextureSrvUavKey, uint32_t> textureSrvUavMap;
		//std::unordered_map<BufferSrvUavKey, uint32_t> bufferSrvUavMap;
		std::unordered_map<TexelBufferSrvUavKey, uint32_t> texelBufferSrvMap;
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
			case CompiledRenderGraph::ResourceKind::texture:
			{
				CompiledRenderGraph::Texture* texture = static_cast<CompiledRenderGraph::Texture*>(resource);
				TextureHandle textureHandle = renderSystem->createTextureHandle(texture->desc());
				if (textureHandle.isValid())
				{
					resourceMap.textures[resourceIndex] = executableRenderGraph->m_textures.size();
					executableRenderGraph->m_textures.push_back(textureHandle);
				}
				else
				{
					diagnosticInfo.errors.push_back({ RenderGraphTranslator::DiagnosticCode::error_create_texture_failed, resourceIndex, invalid_pass });
				}
			}
			break;
			case CompiledRenderGraph::ResourceKind::buffer:
			{
				CompiledRenderGraph::Buffer* buffer = static_cast<CompiledRenderGraph::Buffer*>(resource);
				BufferHandle bufferHandle = renderSystem->createBufferHandle(buffer->desc());
				if (bufferHandle.isValid())
				{
					resourceMap.buffers[resourceIndex] = executableRenderGraph->m_buffers.size();
					executableRenderGraph->m_buffers.push_back(bufferHandle);
				}
				else
				{
					diagnosticInfo.errors.push_back({ RenderGraphTranslator::DiagnosticCode::error_create_buffer_failed, resourceIndex, invalid_pass });
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

	uint32_t GetRenderTargetView(
		RenderGraphTranslator::DiagnosticInfo& diagnosticInfo,
		const ResourceMap& resourceMap,
		ResourceViewMap& resourceViewMap,
		ExecutableRenderGraph* dstRenderGraph,
		const CompiledRenderGraph* srcRenderGraph,
		const CompiledRenderGraph::RTV& rtv,
		RenderSystem* renderSystem)
	{
		VkImage image = VK_NULL_HANDLE;
		const CompiledRenderGraph::Resource* srcResource = srcRenderGraph->resource(rtv.resource);
		switch (srcResource->kind())
		{
		case CompiledRenderGraph::ResourceKind::texture: {
			auto it = resourceMap.textures.find(rtv.resource);
			GAIA_ASSERT(resourceMap.textures.end() != it);
			GAIA_ASSERT(it->second < dstRenderGraph->m_textures.size());
			image = dstRenderGraph->m_textures[it->second].image();
			break;}
		case CompiledRenderGraph::ResourceKind::external_texture: {
			auto it = resourceMap.externalTextures.find(rtv.resource);
			GAIA_ASSERT(resourceMap.externalTextures.end() != it);
			GAIA_ASSERT(it->second < dstRenderGraph->m_externalTextures.size());
			image = dstRenderGraph->m_externalTextures[it->second]->nativeHandle();
			break;}
		default:
			GAIA_ASSERT(false);
		}
		GAIA_ASSERT(VK_NULL_HANDLE != image);

		RtvKey key = { image, rtv.usageDesc };
		auto it = resourceViewMap.rtvMap.find(key);
		if (resourceViewMap.rtvMap.end() == it)
		{
			VkDevice device = renderSystem->device();
			VkImageViewCreateInfo createInfo;
			MapRenderTargetViewDesc(createInfo, rtv.usageDesc, image);
			VkImageView imageView;
			VkResult result = vkCreateImageView(device, &createInfo, nullptr, &imageView);
			if (VK_SUCCESS == result)
			{
				uint32_t viewIndex = dstRenderGraph->m_imageViews.size();
				dstRenderGraph->m_imageViews.push_back(imageView);
				resourceViewMap.rtvMap.insert(std::make_pair(key, viewIndex));
			}
			else
			{
				diagnosticInfo.errors.push_back({ RenderGraphTranslator::DiagnosticCode::error_create_image_view_failed, rtv.resource, invalid_pass });
				return invalid_resource_view;
			}
		}
		else
		{
			return it->second;
		}
	}

	void BuildScenePass(
		RenderGraphTranslator::DiagnosticInfo& diagnosticInfo,
		ExecutableRenderGraph::ScenePass* dstScenePass,
		const CompiledRenderGraph::ScenePass* srcScenePass,
		ExecutableRenderGraph* dstRenderGraph,
		const CompiledRenderGraph* srcRenderGraph,
		const ResourceMap& resourceMap,
		ResourceViewMap& resourceViewMap,
		RenderSystem* renderSystem)
	{
		GAIA_ASSERT(srcScenePass->rtvs().size() <= gaia_max_color_attment_count);

		uint32_t rtvCount = srcScenePass->rtvs().size();
		dstScenePass->m_beginRendering.m_renderingInfo.colorAttachmentCount = rtvCount;
		for (uint32_t i = 0; i < rtvCount; ++i)
		{
			auto& rtv = srcScenePass->rtvs()[i];
			uint32_t viewIndex = GetRenderTargetView(diagnosticInfo, resourceMap, resourceViewMap, dstRenderGraph, srcRenderGraph, rtv, renderSystem);
			auto& colorAttachmentInfo = dstScenePass->m_beginRendering.m_colorAttachmentInfos[i];
			colorAttachmentInfo.imageView = dstRenderGraph->m_imageViews[viewIndex];
			colorAttachmentInfo.

		}
	}

	void BuildPasses(
		RenderGraphTranslator::DiagnosticInfo& diagnosticInfo,
		ExecutableRenderGraph* executableRenderGraph,
		const CompiledRenderGraph* compiledRenderGraph,
		const ResourceMap& resourceMap)
	{
		uint32_t passCount = compiledRenderGraph->passCount();
		for (uint32_t passIndex = 0; passIndex < passCount; ++passIndex)
		{
			const CompiledRenderGraph::Pass* srcPass = compiledRenderGraph->pass(passIndex);
			std::unique_ptr<ExecutableRenderGraph::Pass> executablePass;
			switch (srcPass->kind())
			{
			case RenderPassKind::scene_pass:
				executablePass = std::make_unique<ExecutableRenderGraph::ScenePass>();
				break;
			case RenderPassKind::image_pass:
				executablePass = std::make_unique<ExecutableRenderGraph::ImagePass>();
				break;
			case RenderPassKind::compute_pass:
				executablePass = std::make_unique<ExecutableRenderGraph::ComputePass>();
				break;
			}
			executableRenderGraph->m_passes.push_back(std::move(executablePass));
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

	return diagnosticInfo;
}

END_GAIA_VK1
