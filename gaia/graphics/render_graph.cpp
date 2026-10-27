#include "render_graph.h"
#include <type_traits>
#include <utility>

BEGIN_GAIA

template<typename T>
T* RenderGraph::addPass_(std::string_view name)
{
	static_assert(std::is_base_of_v<Pass, T>, "T must derive from Pass");
	auto pass = std::make_unique<T>(name);
	T* res = pass.get();
	m_passes.push_back(std::move(pass));
	return res;
}

template<typename T, class...Args>
uint32_t RenderGraph::addResource_(Args&&... args)
{
	static_assert(std::is_base_of_v<Resource, T>, "T must derive from Resource");
	uint32_t handle = (uint32_t)m_resources.size();
	auto resource = std::make_unique<T>(std::forward<Args>(args)...);
	m_resources.push_back(std::move(resource));
	return handle;
}

uint32_t RenderGraph::addTexture(std::string_view name, const TextureDesc& desc)
{
	return addResource_<Texture>(name, desc);
}

uint32_t RenderGraph::addBuffer(std::string_view name, const BufferDesc& desc)
{
	return addResource_<Buffer>(name, desc);
}

uint32_t RenderGraph::addExternalTexture(std::string_view name, gaia::Texture* texture)
{
	return addResource_<ExternalTexture>(name, texture);
}

uint32_t RenderGraph::addExternalBuffer(std::string_view name, gaia::Buffer* buffer, ResourceState enterState, ResourceState exitState)
{
	return addResource_<ExternalBuffer>(name, buffer, enterState, exitState);
}

RenderGraph::ScenePass* RenderGraph::addScenePass(std::string_view name)
{
	return addPass_<ScenePass>(name);
}

RenderGraph::ImagePass* RenderGraph::addImagePass(std::string_view name)
{
	return addPass_<ImagePass>(name);	
}

RenderGraph::ComputePass* RenderGraph::addComputePass(std::string_view name)
{
	return addPass_<ComputePass>(name);
}

RenderGraph::RayTracingPass* RenderGraph::addRayTracingPass(std::string_view name)
{
	return addPass_<RayTracingPass>(name);
}

void RenderGraph::reset()
{
	m_resources.clear();
	m_passes.clear();
}

END_GAIA