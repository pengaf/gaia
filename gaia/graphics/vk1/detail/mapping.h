#pragma once

#include "vulkan.h"
#include "../utility.h"
#include "../../common.h"

BEGIN_GAIA_VK1

VkFormat MappingTextureFormat(TextureFormat format);
VkImageUsageFlags MappingTextureUsage(TextureUsage usage);

END_GAIA_VK1
