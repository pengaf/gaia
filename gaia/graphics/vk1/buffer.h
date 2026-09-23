#pragma once
#include "utility.h"
#include "../common.h"
#include "detail/vulkan.h"
#include "detail/vk_mem_alloc.h"

BEGIN_GAIA_VK1

class RenderSystem;

class Buffer
{
	friend class RenderSystem;
public:
	uint64_t size() const
	{
		return m_size;
	}
private:
	Buffer(const Buffer&) = delete;
	Buffer(Buffer&&) = delete;
	Buffer& operator = (const Buffer&) = delete;
	Buffer& operator = (Buffer&&) = delete;
	Buffer(VkBuffer buffer, VmaAllocation allocation, RenderSystem* renderSystem, VkDeviceSize size);
	~Buffer();
private:
	VkBuffer m_buffer{ VK_NULL_HANDLE };
	VmaAllocation m_allocation{ VK_NULL_HANDLE };
	RenderSystem* m_renderSystem{ nullptr };
	VkDeviceSize m_size{ 0 };
	GAIA_ATOMIC_REF_COUNT_IMPL
private:
	RefPtr<Buffer> New(RenderSystem* renderSystem, const BufferDesc& desc);
};

END_GAIA_VK1
