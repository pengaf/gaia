#include "render_graph.h"
#include "render_pass.h"

BEGIN_GAIA


template<typename T>
T* RenderGraph::addPass(std::string_view name)
{
	static_assert(std::is_base_of_v<RenderPass, T>, "T must derive from RenderPass");
	T* pass = new T();
	pass->m_name = name;
	m_renderPasses.push_back(pass);
	return pass;
}

uint32_t RenderGraph::addTexture(std::string_view name, const TextureDesc& desc)
{
	uint32_t handle = m_resources.size();
	TextureInfo* texture = new TextureInfo(name, desc);
	m_resources.push_back(texture);
	return handle;
}

uint32_t RenderGraph::addBuffer(std::string_view name, const BufferDesc& desc)
{
	uint32_t handle = m_resources.size();
	BufferInfo* buffer = new BufferInfo(name, desc);
	m_resources.push_back(buffer);
	return handle;
}

uint32_t RenderGraph::addExternalSrv(std::string_view name, ShaderResourceView* srv, ResourceState oldState)
{
	uint32_t handle = m_resources.size();
	ExternalSrvInfo* externalSrv = new ExternalSrvInfo(name, srv, oldState);
	m_resources.push_back(externalSrv);
}

uint32_t RenderGraph::addExternalUav(std::string_view name, UnorderedAccessView* uav, ResourceState oldState)
{
	uint32_t handle = m_resources.size();
	ExternalUavInfo* externalUav = new ExternalUavInfo(name, uav, oldState);
	m_resources.push_back(externalUav);
}

ScenePass* RenderGraph::addScenePass(std::string_view name)
{
	return addPass<ScenePass>(name);
}

ImagePass* RenderGraph::addImagePass(std::string_view name)
{
	return addPass<ImagePass>(name);	
}

ComputePass* RenderGraph::addComputePass(std::string_view name)
{
	return addPass<ComputePass>(name);
}

RayTracingPass* RenderGraph::addRayTracingPass(std::string_view name)
{
	return addPass<RayTracingPass>(name);
}

END_GAIA