#include "buffer.h"

BEGIN_GAIA_GRAPHICS_VK1

VkBufferUsageFlags MappingBufferUsage(BufferUsage bufferUsage)
{
	VkBufferUsageFlags result = 0;
	if ((bufferUsage & BufferUsage::uniform) != BufferUsage(0))
	{
		result |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	}
	if ((bufferUsage & BufferUsage::storage) != BufferUsage(0))
	{
		result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	}
	if ((bufferUsage & BufferUsage::index) != BufferUsage(0))
	{
		result |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	}
	if ((bufferUsage & BufferUsage::vertex) != BufferUsage(0))
	{
		result |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	}
	if ((bufferUsage & BufferUsage::indirect) != BufferUsage(0))
	{
		result |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
	}
	if ((bufferUsage & BufferUsage::copy_src) != BufferUsage(0))
	{
		result |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	}
	if ((bufferUsage & BufferUsage::copy_dst) != BufferUsage(0))
	{
		result |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}
	if ((bufferUsage & BufferUsage::query_resolve) != BufferUsage(0))
	{
		result |= VK_BUFFER_USAGE_QUERY_RESULT_BUFFER_BIT;
	}
	return result;
};


RefPtr<Buffer> New(RenderSystem* renderSystem, uint64_t size, BufferUsage bufferUsage, CpuAccess cpuAccess)
{
	VkBufferCreateInfo bufferCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = MappingBufferUsage(bufferUsage),
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	VmaAllocationCreateInfo allocationCreateInfo = {};
	allocationCreateInfo.flags = 0;

	switch (cpuAccess) 
	{
	case CpuAccess::none:
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		break;
	case CpuAccess::write:
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		break;
	case CpuAccess::read:
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_TO_CPU;
		allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
		break;
	}

	const uint64_t big_resource_threshold = 1024 * 1024 * 32;
	if (size > big_resource_threshold) 
	{
		allocationCreateInfo.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
	}

	VkBuffer buffer = VK_NULL_HANDLE;
	VmaAllocation allocation = VK_NULL_HANDLE;
	VmaAllocationInfo allocationInfo = {};
	
	VkResult result = vmaCreateBuffer(renderSystem->allocator(), &bufferCreateInfo, &allocationCreateInfo, &m_buffer, &m_allocation, &m_allocInfo);

	if (result != VK_SUCCESS)
	{
		return nullptr;
	}

	return RefPtr<Buffer>(new Buffer(VK_NULL_HANDLE, VMA_NULL_HANDLE, renderSystem, size));
}

Buffer::Buffer(RenderSystem* renderSystem)
	: m_renderSystem(renderSystem)
{
}


END_GAIA_GRAPHICS_VK1