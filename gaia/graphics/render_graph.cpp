#include "render_graph.h"
#include "render_pass.h"

BEGIN_GAIA


template<typename T>
T* RenderGraph::addPass(const char* name)
{
	static_assert(std::is_base_of_v<RenderPass, T>, "T must derive from RenderPass");
	T* pass = new T();
	pass->m_name = m_passNamePool.getElement(name);
	m_renderPasses.push_back(pass);
	return pass;
}

uint32_t RenderGraph::addTexture(const char* name, const TextureDesc& desc)
{
	uint32_t handle = m_resources.size();
	TextureInfo* texture = new TextureInfo(m_resourceNamePool.getElement(name), desc);
	m_resources.push_back(texture);
	return handle;
}

uint32_t RenderGraph::addBuffer(const char* name, const BufferDesc& desc)
{
	uint32_t handle = m_resources.size();
	BufferInfo* buffer = new BufferInfo(m_resourceNamePool.getElement(name), desc);
	m_resources.push_back(buffer);
	return handle;
}

ScenePass* RenderGraph::addScenePass(const char* name)
{
	return addPass<ScenePass>(name);
}

ImagePass* RenderGraph::addImagePass(const char* name)
{
	return addPass<ImagePass>(name);	
}

ComputePass* RenderGraph::addComputePass(const char* name)
{
	return addPass<ComputePass>(name);
}

RayTracingPass* RenderGraph::addRayTracingPass(const char* name)
{
	return addPass<RayTracingPass>(name);
}

END_GAIA