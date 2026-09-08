#include "render_graph_builder.h"
#include "render_pass.h"

BEGIN_GAIA_RG
//
//namespace
//{
//	template<typename T>
//	T* CreatePass(const char* name, pafcore::StringPool& passNamePool, std::vector<RenderPass*>& renderPasses)
//	{
//		static_assert(std::is_base_of_v<RenderPass, T>, "T must derive from RenderPass");
//		T* pass = new T();
//		pass->m_name = passNamePool.getElement(name);
//		renderPasses.push_back(pass);
//		return pass;
//	};
//}

template<typename T>
T* RenderGraphBuilder::addPass(const char* name)
{
	static_assert(std::is_base_of_v<RenderPass, T>, "T must derive from RenderPass");
	T* pass = new T();
	pass->m_name = m_passNamePool.getElement(name);
	m_renderPasses.push_back(pass);
	return pass;
}

TextureHandle RenderGraphBuilder::createTexture(const TextureDesc& desc)
{
	//auto it = m_resourceNamePool.find(desc.name);
	//GAIA_ASSERT(m_resourceNamePool.end() == it);
	TextureHandle handle;
	handle.index = m_textures.size();
	m_textures.push_back(desc);
	m_textures.back().name = m_resourceNamePool.getElement(desc.name);
	return handle;
}

BufferHandle RenderGraphBuilder::createBuffer(const BufferDesc& desc)
{
	//auto it = m_resourceNamePool.find(desc.name);
	//GAIA_ASSERT(m_resourceNamePool.end() == it);
	BufferHandle handle;
	handle.index = m_buffers.size();
	m_buffers.push_back(desc);
	m_buffers.back().name = m_resourceNamePool.getElement(desc.name);
	return handle;
}

ScenePass* RenderGraphBuilder::addScenePass(const char* name)
{
	return addPass<ScenePass>(name);
}

ImagePass* RenderGraphBuilder::addImagePass(const char* name)
{
	return addPass<ImagePass>(name);	
}

ComputePass* RenderGraphBuilder::addComputePass(const char* name)
{
	return addPass<ComputePass>(name);
}

RayTracingPass* RenderGraphBuilder::addRayTracingPass(const char* name)
{
	return addPass<RayTracingPass>(name);
}

END_GAIA_RG