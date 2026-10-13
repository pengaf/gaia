#include "compiled_render_graph.h"
#include <type_traits>
#include <utility>

BEGIN_GAIA

template<typename T>
T* CompiledRenderGraph::addPass(std::string_view name)
{
	static_assert(std::is_base_of_v<Pass, T>, "T must derive from Pass");
	auto pass = std::make_unique<T>(name);
	T* res = pass.get();
	m_passes.push_back(std::move(pass));
	return res;
}

template<typename T, class...Args>
uint32_t CompiledRenderGraph::addResource(Args&&... args)
{
	static_assert(std::is_base_of_v<Resource, T>, "T must derive from Resource");
	uint32_t handle = (uint32_t)m_resources.size();
	auto resource = std::make_unique<T>(std::forward<Args>(args)...);
	m_resources.push_back(std::move(resource));
	return handle;
}

uint32_t CompiledRenderGraph::addTexture(std::string_view name, const TextureDesc& desc)
{
	return addResource<Texture>(name, desc);
}

uint32_t CompiledRenderGraph::addBuffer(std::string_view name, const BufferDesc& desc)
{
	return addResource<Buffer>(name, desc);
}

uint32_t CompiledRenderGraph::cloneExternalResource(ExternalResource* src)
{
	switch (src->kind())
	{
		case ResourceKind::external_texture:
			return addResource<ExternalResource>(*static_cast<ExternalResource*>(src));
		case ResourceKind::external_buffer:
			return addResource<ExternalResource>(*static_cast<ExternalResource*>(src));
		default:
			GAIA_ASSERT(false);
			return invalid_resource;
	}
}

//uint32_t CompiledRenderGraph::addExternalTexture(std::string_view name, Texture* texture, ResourceState oldState)
//{
//	return addResource<ExternalResource>(name, texture, oldState);
//}
//
//uint32_t CompiledRenderGraph::addExternalBuffer(std::string_view name, Buffer* buffer, ResourceState oldState)
//{
//	return addResource<ExternalResource>(name, buffer, oldState);
//}

CompiledRenderGraph::ScenePass* CompiledRenderGraph::addScenePass(std::string_view name)
{
	return addPass<ScenePass>(name);
}

CompiledRenderGraph::ImagePass* CompiledRenderGraph::addImagePass(std::string_view name)
{
	return addPass<ImagePass>(name);
}

CompiledRenderGraph::ComputePass* CompiledRenderGraph::addComputePass(std::string_view name)
{
	return addPass<ComputePass>(name);
}

CompiledRenderGraph::RayTracingPass* CompiledRenderGraph::addRayTracingPass(std::string_view name)
{
	return addPass<RayTracingPass>(name);
}

void CompiledRenderGraph::reset()
{
	m_resources.clear();
	m_passes.clear();
}

END_GAIA