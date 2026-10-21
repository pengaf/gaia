#include "executable_render_graph.h"
#include "render_system.h"

BEGIN_GAIA_VK1

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


END_GAIA_VK1
