#include "executable_render_graph.h"
#include "render_system.h"

BEGIN_GAIA_VK1

template<typename T>
T* ExecutableRenderGraph::addPass_(std::string_view name)
{
	static_assert(std::is_base_of_v<Pass, T>, "T must derive from Pass");
	auto pass = std::make_unique<T>();
	T* res = pass.get();
	res->name(std::string(name));
	m_passes.push_back(std::move(pass));
	return res;
}

void ExecutableRenderGraph::reset(RenderSystem* renderSystem)
{
	VkDevice device = renderSystem->device();
	for (VkImageView imageView : m_imageViews)
	{
		vkDestroyImageView(device, imageView, nullptr);
	}
	for (TextureHandle texture : m_textures)
	{
		renderSystem->destroyTextureHandle(texture);
	}
	for (BufferHandle buffer: m_buffers)
	{
		renderSystem->destroyBufferHandle(buffer);
	}
}

ExecutableRenderGraph::ScenePass* ExecutableRenderGraph::addScenePass(std::string_view name)
{
	return addPass_<ScenePass>(name);
}

ExecutableRenderGraph::ImagePass* ExecutableRenderGraph::addImagePass(std::string_view name)
{
	return addPass_<ImagePass>(name);
}

ExecutableRenderGraph::ComputePass* ExecutableRenderGraph::addComputePass(std::string_view name)
{
	return addPass_<ComputePass>(name);
}

END_GAIA_VK1
