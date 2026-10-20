#pragma once

#include "vulkan.h"
#include "../utility.h"
#include "../../common.h"

BEGIN_GAIA_VK1

VkBufferUsageFlags MappingBufferUsage(BufferUsage bufferUsage);
VkImageUsageFlags MappingTextureUsage(TextureUsage usage);
VkFormat MappingTextureFormat(TextureFormat format);
VkImageType MappingTextureType(TextureDimension dim);

END_GAIA_VK1
