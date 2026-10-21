#include "mapping.h"

BEGIN_GAIA_VK1

VkBufferUsageFlags MapBufferUsage(BufferUsage bufferUsage)
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


VkImageUsageFlags MapTextureUsage(TextureUsage textureUsage)
{
    VkImageUsageFlags flags = 0;

    if ((textureUsage & TextureUsage::copy_src) != TextureUsage(0))
    {
        flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }

    if ((textureUsage & TextureUsage::copy_dst) != TextureUsage(0))
    {
        flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    if ((textureUsage & TextureUsage::shader_resource) != TextureUsage(0))
    {
        flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
    }

    if ((textureUsage & TextureUsage::unordered_access) != TextureUsage(0))
    {
        flags |= VK_IMAGE_USAGE_STORAGE_BIT;
    }

    if ((textureUsage & TextureUsage::render_target) != TextureUsage(0))
    {
        flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    if ((textureUsage & TextureUsage::depth_stencil) != TextureUsage(0))
    {
        flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }

    if ((textureUsage & TextureUsage::input) != TextureUsage(0))
    {
        flags |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    }

    if ((textureUsage & TextureUsage::transient) != TextureUsage(0))
    {
        flags |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
    }

    return flags;
}


static VkFormat s_formats[] =
{
    VK_FORMAT_UNDEFINED,
    // 8 BIT FORMATS
    VK_FORMAT_R8_UNORM,
    VK_FORMAT_R8_SNORM,
    VK_FORMAT_R8_UINT,
    VK_FORMAT_R8_SINT, 
    // 16 BIT FORMATS
    VK_FORMAT_R16_UNORM,
    VK_FORMAT_R16_SNORM,
    VK_FORMAT_R16_UINT,
    VK_FORMAT_R16_SINT,
    VK_FORMAT_R16_SFLOAT,
    VK_FORMAT_R8G8_UNORM,
    VK_FORMAT_R8G8_SNORM,
    VK_FORMAT_R8G8_UINT,
    VK_FORMAT_R8G8_SINT, 
    // 32 BIT FORMATS
    VK_FORMAT_R32_UINT,
    VK_FORMAT_R32_SINT,
    VK_FORMAT_R32_SFLOAT,
    VK_FORMAT_R16G16_UNORM,
    VK_FORMAT_R16G16_SNORM,
    VK_FORMAT_R16G16_UINT,
    VK_FORMAT_R16G16_SINT,
    VK_FORMAT_R16G16_SFLOAT,
    VK_FORMAT_R8G8B8A8_UNORM,
    VK_FORMAT_R8G8B8A8_SRGB,
    VK_FORMAT_R8G8B8A8_SNORM,
    VK_FORMAT_R8G8B8A8_UINT,
    VK_FORMAT_R8G8B8A8_SINT,
    VK_FORMAT_B8G8R8A8_UNORM,
    VK_FORMAT_B8G8R8A8_SRGB,
    // PACKED 32 BIT FORMATS
    VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
    VK_FORMAT_A2R10G10B10_UINT_PACK32,
    VK_FORMAT_A2R10G10B10_UNORM_PACK32,
    VK_FORMAT_B10G11R11_UFLOAT_PACK32,
    // 64 BIT FORMATS
    VK_FORMAT_R32G32_UINT,
    VK_FORMAT_R32G32_SINT,
    VK_FORMAT_R32G32_SFLOAT,
    VK_FORMAT_R16G16B16A16_UNORM,
    VK_FORMAT_R16G16B16A16_SNORM,
    VK_FORMAT_R16G16B16A16_UINT,
    VK_FORMAT_R16G16B16A16_SINT,
    VK_FORMAT_R16G16B16A16_SFLOAT,
    // 128 BIT FORMATS
    VK_FORMAT_R32G32B32A32_UINT,
    VK_FORMAT_R32G32B32A32_SINT,
    VK_FORMAT_R32G32B32A32_SFLOAT,
    // DEPTH/STENCIL FORMATS
    VK_FORMAT_S8_UINT,
    VK_FORMAT_D16_UNORM,
    VK_FORMAT_X8_D24_UNORM_PACK32,
    VK_FORMAT_D24_UNORM_S8_UINT,
    VK_FORMAT_D32_SFLOAT,
    VK_FORMAT_D32_SFLOAT_S8_UINT,
    // BC COMPRESSED FORMATS
    VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
    VK_FORMAT_BC1_RGBA_SRGB_BLOCK,
    VK_FORMAT_BC2_UNORM_BLOCK,
    VK_FORMAT_BC2_SRGB_BLOCK,
    VK_FORMAT_BC3_UNORM_BLOCK,
    VK_FORMAT_BC3_SRGB_BLOCK,
    VK_FORMAT_BC4_UNORM_BLOCK,
    VK_FORMAT_BC4_SNORM_BLOCK,
    VK_FORMAT_BC5_UNORM_BLOCK,
    VK_FORMAT_BC5_SNORM_BLOCK,
    VK_FORMAT_BC6H_UFLOAT_BLOCK,
    VK_FORMAT_BC6H_SFLOAT_BLOCK,
    VK_FORMAT_BC7_UNORM_BLOCK,
    VK_FORMAT_BC7_SRGB_BLOCK,
    // ETC2 COMPRESSED FORMATS
    VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,
    VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,
    VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,
    VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,
    VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,
    VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,
    VK_FORMAT_EAC_R11_UNORM_BLOCK,
    VK_FORMAT_EAC_R11_SNORM_BLOCK,
    VK_FORMAT_EAC_R11G11_UNORM_BLOCK,
    VK_FORMAT_EAC_R11G11_SNORM_BLOCK,
    // ASTC COMPRESSED FORMATS_BLOCK 
    VK_FORMAT_ASTC_4x4_UNORM_BLOCK,
    VK_FORMAT_ASTC_4x4_SRGB_BLOCK,
    VK_FORMAT_ASTC_5x4_UNORM_BLOCK,
    VK_FORMAT_ASTC_5x4_SRGB_BLOCK,
    VK_FORMAT_ASTC_5x5_UNORM_BLOCK,
    VK_FORMAT_ASTC_5x5_SRGB_BLOCK,
    VK_FORMAT_ASTC_6x5_UNORM_BLOCK,
    VK_FORMAT_ASTC_6x5_SRGB_BLOCK,
    VK_FORMAT_ASTC_6x6_UNORM_BLOCK,
    VK_FORMAT_ASTC_6x6_SRGB_BLOCK,
    VK_FORMAT_ASTC_8x5_UNORM_BLOCK,
    VK_FORMAT_ASTC_8x5_SRGB_BLOCK,
    VK_FORMAT_ASTC_8x6_UNORM_BLOCK,
    VK_FORMAT_ASTC_8x6_SRGB_BLOCK,
    VK_FORMAT_ASTC_8x8_UNORM_BLOCK,
    VK_FORMAT_ASTC_8x8_SRGB_BLOCK,
    VK_FORMAT_ASTC_10x5_UNORM_BLOCK,
    VK_FORMAT_ASTC_10x5_SRGB_BLOCK,
    VK_FORMAT_ASTC_10x6_UNORM_BLOCK,
    VK_FORMAT_ASTC_10x6_SRGB_BLOCK,
    VK_FORMAT_ASTC_10x8_UNORM_BLOCK,
    VK_FORMAT_ASTC_10x8_SRGB_BLOCK,
    VK_FORMAT_ASTC_10x10_UNORM_BLOCK,
    VK_FORMAT_ASTC_10x10_SRGB_BLOCK,
    VK_FORMAT_ASTC_12x10_UNORM_BLOCK,
    VK_FORMAT_ASTC_12x10_SRGB_BLOCK,
    VK_FORMAT_ASTC_12x12_UNORM_BLOCK,
    VK_FORMAT_ASTC_12x12_SRGB_BLOCK,
};

VkFormat MapTextureFormat(TextureFormat format)
{
    static_assert(sizeof(s_formats) / sizeof(s_formats[0]) == uint32_t(TextureFormat::count));
    if (uint32_t(format) < sizeof(s_formats) / sizeof(s_formats[0]))
    {
        return s_formats[uint32_t(format)];
    }
    return VK_FORMAT_UNDEFINED;
}

VkImageType MapTextureDimension(TextureDimension dim)
{
    switch (dim)
    {
    case TextureDimension::tex1d:
        return VK_IMAGE_TYPE_1D;
    case TextureDimension::tex2d:
        return VK_IMAGE_TYPE_2D;
    case TextureDimension::tex3d:
        return VK_IMAGE_TYPE_3D;
    default:
        GAIA_ASSERT(false);
        return VK_IMAGE_TYPE_MAX_ENUM;
    }
}

VkImageViewType MapTextureViewDimension(TextureViewDimension dim)
{
	switch (dim)
	{
	case TextureViewDimension::texture_view_1d:
		return VK_IMAGE_VIEW_TYPE_1D;
	case TextureViewDimension::texture_view_2d:
		return VK_IMAGE_VIEW_TYPE_2D;
	case TextureViewDimension::texture_view_3d:
		return VK_IMAGE_VIEW_TYPE_3D;
	case TextureViewDimension::texture_view_cube:
		return VK_IMAGE_VIEW_TYPE_CUBE;
	case TextureViewDimension::texture_view_1d_array:
		return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
	case TextureViewDimension::texture_view_2d_array:
		return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
	case TextureViewDimension::texture_view_cube_array:
		return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
	default:
		return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
	}
}

VkAccessFlags MapBufferState(ResourceState state)
{
    switch (state)
    {
    case ResourceState::undefined:
        return 0;

    case ResourceState::common:
        return VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;

    case ResourceState::copy_src:
        return VK_ACCESS_TRANSFER_READ_BIT;

    case ResourceState::copy_dst:
        return VK_ACCESS_TRANSFER_WRITE_BIT;

    case ResourceState::shader_read:
        return VK_ACCESS_SHADER_READ_BIT;

    case ResourceState::shader_write:
        return VK_ACCESS_SHADER_WRITE_BIT;

    case ResourceState::shader_read_write:
        return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;

    case ResourceState::vertex_buffer:
        return VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;

    case ResourceState::index_buffer:
        return VK_ACCESS_INDEX_READ_BIT;

    case ResourceState::uniform_buffer:
        return VK_ACCESS_UNIFORM_READ_BIT;

    case ResourceState::indirect_args:
        return VK_ACCESS_INDIRECT_COMMAND_READ_BIT;

    case ResourceState::query_resolve:
        return VK_ACCESS_TRANSFER_WRITE_BIT; 

    case ResourceState::render_target:
    case ResourceState::depth_stencil_write:
    case ResourceState::depth_stencil_read:
    case ResourceState::present:
        GAIA_ASSERT(false);
        return 0;

    default:
        GAIA_ASSERT(false);
        return 0;
    }
}


void MapTextureState(VkAccessFlags& accessFlags, VkImageLayout& imageLayout, ResourceState state)
{
    switch (state)
    {
    case ResourceState::undefined:
        imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        accessFlags = 0;
        break;

    case ResourceState::common:
        imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        accessFlags = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
        break;

    case ResourceState::copy_src:
        imageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        accessFlags = VK_ACCESS_TRANSFER_READ_BIT;
        break;

    case ResourceState::copy_dst:
        imageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        accessFlags = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;

    case ResourceState::shader_read:
        imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        accessFlags = VK_ACCESS_SHADER_READ_BIT;
        break;

    case ResourceState::shader_write:
        imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        accessFlags = VK_ACCESS_SHADER_WRITE_BIT;
        break;

    case ResourceState::shader_read_write:
        imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        accessFlags = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        break;

    case ResourceState::render_target:
        imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        accessFlags = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;

    case ResourceState::depth_stencil_write:
        imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        accessFlags = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;

    case ResourceState::depth_stencil_read:
        imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        accessFlags = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_SHADER_READ_BIT;
        break;

    case ResourceState::present:
        imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        accessFlags = 0;
        break;

    case ResourceState::vertex_buffer:
    case ResourceState::index_buffer:
    case ResourceState::uniform_buffer:
    case ResourceState::indirect_args:
    case ResourceState::query_resolve:
        GAIA_ASSERT(false);
        imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        accessFlags = 0;
        break;

    default:
        GAIA_ASSERT(false);
        imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        accessFlags = 0;
        break;
    }
}

void MapRenderTargetViewDesc(VkImageViewCreateInfo& dst, const RenderTargetViewDesc& src, VkImage image)
{
    dst.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    dst.pNext = nullptr;
    dst.flags = 0;
    dst.image = image, 
    dst.viewType = MapTextureViewDimension(src.dimension);
    dst.format = MapTextureFormat(src.format);
    dst.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    dst.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    dst.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    dst.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    dst.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    dst.subresourceRange.baseMipLevel = src.baseMipLevel;
    dst.subresourceRange.levelCount = src.levelCount;
    dst.subresourceRange.baseArrayLayer = src.baseArrayLayer;
    dst.subresourceRange.layerCount = src.layerCount;
}

END_GAIA_VK1
