#pragma once
#include "utility.h"
#include "../common.h"
#include "detail/vulkan.h"
#include "detail/vk_mem_alloc.h"

BEGIN_GAIA_VK1

class Buffer
{
	friend class RenderSystem;
public:
	uint64_t size() const
	{
		return m_desc.size;
	}
private:
	Buffer(const Buffer&) = delete;
	Buffer(Buffer&&) = delete;
	Buffer& operator = (const Buffer&) = delete;
	Buffer& operator = (Buffer&&) = delete;
	Buffer(VkBuffer buffer, VmaAllocation allocation, RenderSystem* renderSystem, const BufferDesc& desc);
	~Buffer();
protected:
	VkBuffer m_buffer{ VK_NULL_HANDLE };
	VmaAllocation m_allocation{ VK_NULL_HANDLE };
	RenderSystem* m_renderSystem{ nullptr };
public:
	GAIA_ARG_RO(BufferDesc, desc);
protected:
	GAIA_ATOMIC_REF_COUNT_IMPL
protected:
	RefPtr<Buffer> New(RenderSystem* renderSystem, const BufferDesc& desc);
};

class Texture
{
	friend class RenderSystem;
private:
	Texture(const Texture&) = delete;
	Texture(Texture&&) = delete;
	Texture& operator = (const Texture&) = delete;
	Texture& operator = (Texture&&) = delete;
	Texture(VkImage image, VmaAllocation allocation, RenderSystem* renderSystem, const TextureDesc& desc);
	~Texture();
private:
	VkImage m_image{ VK_NULL_HANDLE };
	VmaAllocation m_allocation{ VK_NULL_HANDLE };
	RenderSystem* m_renderSystem{ nullptr };
public:
	GAIA_ARG_RO(TextureDesc, desc);
protected:
	GAIA_ATOMIC_REF_COUNT_IMPL
protected:
	RefPtr<Texture> New(RenderSystem* renderSystem, const TextureDesc& desc);
};


END_GAIA_VK1