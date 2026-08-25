#include "buffer.h"
#include "render_system.h"

BEGIN_GAIA_VK1

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
		result |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}
	return result;
};


RefPtr<Buffer> Buffer::New(RenderSystem* renderSystem, const BufferDesc& desc)
{
	VkBufferCreateInfo bufferCreateInfo
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = desc.size,
		.usage = MappingBufferUsage(desc.bufferUsage),
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	VmaAllocationCreateInfo allocationCreateInfo = {};
	allocationCreateInfo.flags = 0;

	switch (desc.cpuAccess)
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
	if (desc.size > big_resource_threshold)
	{
		allocationCreateInfo.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
	}

	VkBuffer buffer = VK_NULL_HANDLE;
	VmaAllocation allocation = VK_NULL_HANDLE;
	VmaAllocationInfo allocationInfo = {};
	
	VkResult result = vmaCreateBuffer(renderSystem->allocator(), &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation, &allocationInfo);

	if (result != VK_SUCCESS)
	{
		return nullptr;
	}

	return RefPtr<Buffer>(new Buffer(VK_NULL_HANDLE, allocation, renderSystem, desc.size));
}

Buffer::Buffer(VkBuffer buffer, VmaAllocation allocation, RenderSystem* renderSystem, VkDeviceSize size) :
	m_buffer(buffer),
	m_allocation(allocation),
	m_renderSystem(renderSystem),
	m_size(size)
{
}

END_GAIA_VK1