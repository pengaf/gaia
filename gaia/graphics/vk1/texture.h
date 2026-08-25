#pragma once
#include "utility.h"
#include "../common.h"
#include "detail/vulkan.h"
#include "detail/vk_mem_alloc.h"

BEGIN_GAIA_VK1

class Texture
{
	friend class RenderSystem;
public:
	uint64_t size() const
	{
		return m_size;
	}
private:
	Texture(const Texture&) = delete;
	Texture(Texture&&) = delete;
	Texture& operator = (const Texture&) = delete;
	Texture& operator = (Texture&&) = delete;
	Texture(
		RenderSystem* renderSystem,
		VkImage image, 
		VmaAllocation alloc, 
		//VmaAllocationInfo allocInfo, 
		uint32_t width, 
		uint32_t height, 
		uint32_t depth,
		uint32_t mips, 
		uint32_t layers,
		TextureDimension dim,
		TextureFormat fmt);
	~Texture();
private:
	VkImage m_image{ VK_NULL_HANDLE };
	VmaAllocation m_allocation{ VK_NULL_HANDLE };
	RenderSystem* m_renderSystem{ nullptr };
	VkDeviceSize m_size{ 0 };
	uint32_t m_width{ 0 };
	uint32_t m_height{ 0 };
	uint32_t m_depth{ 0 };
	uint32_t m_mipLevels{ 1 };
	uint32_t m_arrayLayers{ 1 };
	TextureDimension m_dimension;
	TextureFormat m_format;
	GAIA_ATOMIC_REF_COUNT_IMPL
private:
	RefPtr<Texture> New(RenderSystem* renderSystem, const TextureDesc& desc);
};


END_GAIA_VK1