#include "resource.h"
#include "detail/mapping.h"
#include "render_system.h"

BEGIN_GAIA_VK1

Buffer::Buffer(VkBuffer buffer, VmaAllocation allocation, RenderSystem* renderSystem, const BufferDesc& desc) :
    m_buffer(buffer),
    m_allocation(allocation),
    m_renderSystem(renderSystem),
    m_desc(desc)
{}

Buffer::~Buffer()
{
    if (m_buffer != VK_NULL_HANDLE)
    {
		GAIA_ASSERT(nullptr != m_renderSystem && nullptr != m_renderSystem->allocator() && nullptr != m_allocation);
        vmaDestroyBuffer(m_renderSystem->allocator(), m_buffer, m_allocation);
        //m_buffer = VK_NULL_HANDLE;
        //m_allocation = VK_NULL_HANDLE;
    }
}

RefPtr<Buffer> Buffer::New(RenderSystem* renderSystem, const BufferDesc& desc)
{
    BufferHandle bufferHandle = renderSystem->createBufferHandle(desc);
	if (bufferHandle.isValid())
	{
		return RefPtr<Buffer>(new Buffer(bufferHandle.buffer(), bufferHandle.allocation(), renderSystem, desc));
        m_allocation = VK_NULL_HANDLE;
    }
}

RefPtr<Buffer> Buffer::New(RenderSystem* renderSystem, const BufferDesc& desc)
{
    BufferHandle bufferHandle = renderSystem->createBufferHandle(desc);
	if (bufferHandle.isValid())
	{
		return RefPtr<Buffer>(new Buffer(bufferHandle.buffer(), bufferHandle.allocation(), renderSystem, desc));
	}
    else
    {
        return nullptr;
    }
}

Texture::Texture(VkImage image, VmaAllocation alloc, RenderSystem* renderSystem, const TextureDesc& desc) : 
    m_image(image),
    m_allocation(alloc),
    m_renderSystem(renderSystem),
    m_desc(desc)
{}

Texture::~Texture()
{
    if (m_image != VK_NULL_HANDLE)
    {
        GAIA_ASSERT(nullptr != m_renderSystem && nullptr != m_renderSystem->allocator() && nullptr != m_allocation);
        vmaDestroyImage(m_renderSystem->allocator(), m_image, m_allocation);
        //m_image = VK_NULL_HANDLE;
        //m_allocation = VK_NULL_HANDLE;
    }
}

RefPtr<Texture> Texture::New(
    RenderSystem* renderSystem,
    const TextureDesc& desc)
{
	TextureHandle textureHandle = renderSystem->createTextureHandle(desc);
    if (textureHandle.isValid())
    {
        return RefPtr<Texture>(new Texture(textureHandle.image(), textureHandle.allocation(), renderSystem, desc));
    }
    else
    {
        return nullptr;
    }
}

END_GAIA_VK1