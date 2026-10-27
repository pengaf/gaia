#pragma once

#include "vulkan.h"
#include "../utility.h"
#include "../../common.h"

BEGIN_GAIA_VK1

VkBufferUsageFlags MapBufferUsage(BufferUsage bufferUsage);
VkImageUsageFlags MapTextureUsage(TextureUsage usage);
VkFormat MapTextureFormat(TextureFormat format);
VkImageAspectFlags MapTextureAspect(TextureFormat format);

VkImageType MapTextureDimension(TextureDimension dim);
VkImageViewType MapTextureViewDimension(TextureViewDimension dim);

VkAccessFlags MapBufferState(ResourceState state);
void MapTextureState(VkAccessFlags& accessFlags, VkImageLayout& imageLayout, ResourceState state);
void MapRenderTargetViewDesc(VkImageViewCreateInfo& dst, const RenderTargetViewDesc& src, VkImage image);
void MapDepthStencilViewDesc(VkImageViewCreateInfo& dst, const DepthStencilViewDesc& src, VkImage image);


END_GAIA_VK1
