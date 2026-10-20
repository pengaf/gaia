#include "render_graph_translator.h"
#include "render_system.h"
#include "../compiled_render_graph.h"
#include <unordered_map>

BEGIN_GAIA_VK1

namespace
{
	constexpr uint32_t invalid_pass = UINT32_MAX;
	constexpr uint32_t invalid_resource = CompiledRenderGraph::invalid_resource;

	struct ResourceMap
	{
		std::unordered_map<uint32_t, uint32_t> textures;
		std::unordered_map<uint32_t, uint32_t> buffers;
	};

	ResourceMap BuildResources(
		RenderGraphTranslator::ErrorInfo& errorInfo,
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
					errorInfo.errors.push_back({ RenderGraphTranslator::ErrorCode::create_texture_failed, resourceIndex, invalid_pass });
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
					errorInfo.errors.push_back({ RenderGraphTranslator::ErrorCode::create_buffer_failed, resourceIndex, invalid_pass });
				}
			}
			break;
			case CompiledRenderGraph::ResourceKind::external_texture:
			{
				CompiledRenderGraph::ExternalTexture* externalTexture = static_cast<CompiledRenderGraph::ExternalTexture*>(resource);
				Texture* texture = externalTexture->texture();
				GAIA_ASSERT(texture != nullptr);
				resourceMap.textures[resourceIndex] = executableRenderGraph->m_externalTextures.size();
				executableRenderGraph->m_externalTextures.push_back(RefPtr<Texture>(texture));
			}
			break;
			case CompiledRenderGraph::ResourceKind::external_buffer:
			{
				CompiledRenderGraph::ExternalBuffer* externalBuffer = static_cast<CompiledRenderGraph::ExternalBuffer*>(resource);
				Buffer* buffer = externalBuffer->buffer();
				GAIA_ASSERT(buffer != nullptr);
				resourceMap.buffers[resourceIndex] = executableRenderGraph->m_externalBuffers.size();
				executableRenderGraph->m_externalBuffers.push_back(RefPtr<Buffer>(buffer));
			}
			default:
				GAIA_ASSERT(false && "Unknown resource kind");
				break;
			}
		}
		return resourceMap;
	}

	void BuildScenePass(
		RenderGraphTranslator::ErrorInfo& errorInfo,
		ExecutableRenderGraph::ScenePass& dstScenePass,
		const CompiledRenderGraph::ScenePass& srcScenePass,
		ExecutableRenderGraph* dstRenderGraph,
		const CompiledRenderGraph* srcRenderGraph,
		const ResourceMap& resourceMap)
	{
		GAIA_ASSERT(srcScenePass.rtvs().size() <= gaia_max_color_attment_count);

		for (auto& rtv : srcScenePass.rtvs())
		{
			auto* srcResource = srcRenderGraph->resource(rtv.resource);
			GAIA_ASSERT(srcResource != nullptr && srcResource->isTexture());
			switch (srcResource->kind())
			{
			case CompiledRenderGraph::ResourceKind::texture:
				{
					CompiledRenderGraph::Texture* texture = static_cast<CompiledRenderGraph::Texture*>(srcResource);
					auto it = resourceMap.textures.find(rtv.resource);
					if (it != resourceMap.textures.end())
					{
						uint32_t dstResourceIndex = it->second;
					}
				}
				break;
			case CompiledRenderGraph::ResourceKind::external_texture:
				break;
			}
		}
	}
				dstScenePass.m_rtvs.push_back({ dstResourceIndex, rtv.usageDesc });
			}
			else
			{
				errorInfo.errors.push_back({ RenderGraphTranslator::ErrorCode::success, rtv.resource, invalid_pass });
			}
		}
		m_beginRendering
	}

	void BuildPasses(
		RenderGraphTranslator::ErrorInfo& errorInfo,
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
			default:
				errorInfo.errors.push_back({ RenderGraphTranslator::ErrorCode::success, invalid_resource, passIndex });
				continue;
			}
			executableRenderGraph->m_passes.push_back(std::move(executablePass));
		}
	}
}


RenderGraphTranslator::ErrorInfo RenderGraphTranslator::translate(ExecutableRenderGraph* executableRenderGraph, const CompiledRenderGraph* compiledRenderGraph, RenderSystem* renderSystem)
{
	RenderGraphTranslator::ErrorInfo errorInfo;
	//executableRenderGraph->clear();
	ResourceMap resourceMap = BuildResources(errorInfo, executableRenderGraph, compiledRenderGraph, renderSystem);
	return errorInfo;
}

END_GAIA_VK1