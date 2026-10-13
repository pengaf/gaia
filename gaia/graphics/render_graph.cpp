#include "render_graph.h"
#include <type_traits>
#include <utility>

BEGIN_GAIA

template<typename T>
T* RenderGraph::addPass(std::string_view name)
{
	static_assert(std::is_base_of_v<Pass, T>, "T must derive from Pass");
	auto pass = std::make_unique<T>(name);
	T* res = pass.get();
	m_passes.push_back(std::move(pass));
	return res;
}

template<typename T, class...Args>
uint32_t RenderGraph::addResource(Args&&... args)
{
	static_assert(std::is_base_of_v<Resource, T>, "T must derive from Resource");
	uint32_t handle = (uint32_t)m_resources.size();
	auto resource = std::make_unique<T>(std::forward<Args>(args)...);
	m_resources.push_back(std::move(resource));
	return handle;
}

uint32_t RenderGraph::addTexture(std::string_view name, const TextureDesc& desc)
{
	return addResource<Texture>(name, desc);
}

uint32_t RenderGraph::addBuffer(std::string_view name, const BufferDesc& desc)
{
	return addResource<Buffer>(name, desc);
}

uint32_t RenderGraph::addExternalTexture(std::string_view name, Texture* texture, ResourceState oldState)
{
	return addResource<ExternalResource>(name, texture, oldState);
}

uint32_t RenderGraph::addExternalBuffer(std::string_view name, Buffer* buffer, ResourceState oldState)
{
	return addResource<ExternalResource>(name, buffer, oldState);
}

RenderGraph::ScenePass* RenderGraph::addScenePass(std::string_view name)
{
	return addPass<ScenePass>(name);
}

RenderGraph::ImagePass* RenderGraph::addImagePass(std::string_view name)
{
	return addPass<ImagePass>(name);	
}

RenderGraph::ComputePass* RenderGraph::addComputePass(std::string_view name)
{
	return addPass<ComputePass>(name);
}

RenderGraph::RayTracingPass* RenderGraph::addRayTracingPass(std::string_view name)
{
	return addPass<RayTracingPass>(name);
}

void RenderGraph::reset()
{
	m_resources.clear();
	m_passes.clear();
}

END_GAIA