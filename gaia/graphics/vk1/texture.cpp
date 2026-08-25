#include "texture.h"
#include "detail/mapping.h"
#include "render_system.h"

BEGIN_GAIA_VK1


VkImageType MappingImageType(TextureDimension dim)
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
        return VK_IMAGE_TYPE_2D;
    }
}

RefPtr<Texture> Texture::New(
    RenderSystem* renderSystem,
    const TextureDesc& desc)
    //uint32_t width, 
    //uint32_t height, 
    //uint32_t depth,
    //uint32_t mipLevels, 
    //uint32_t arrayLayers,
    //TextureDimension dimension, 
    //TextureFormat format,
    //TextureUsage textureUsage, 
    //CpuAccess cpuAccess)
{
    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = MappingImageType(desc.dimension);
    imageCreateInfo.format = MappingTextureFormat(desc.format);
    imageCreateInfo.extent = { desc.width, desc.height, desc.depth };
    imageCreateInfo.mipLevels = desc.mipLevels;
    imageCreateInfo.arrayLayers = desc.arrayLayers;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage = MappingTextureUsage(desc.textureUsage);
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.flags = 0;

    switch (desc.cpuAccess)
    {
    case CpuAccess::none:
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        break;
    case CpuAccess::write:
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
        break;
    case CpuAccess::read:
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_TO_CPU;
        allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
        imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
        break;
    }

    // 大资源启用独立内存分配
    VkDeviceSize totalBytes = static_cast<VkDeviceSize>(desc.width) * desc.height * desc.depth * desc.arrayLayers * desc.mipLevels;
    const VkDeviceSize bigResourceThreshold = 1024ULL * 1024ULL * 32ULL;
    if (totalBytes > bigResourceThreshold)
    {
        allocInfo.flags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
    }

    VkImage image = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    VmaAllocationInfo allocResultInfo{};

    VkResult ret = vmaCreateImage(renderSystem->allocator(),
        &imageCreateInfo,
        &allocInfo,
        &image,
        &allocation,
        &allocResultInfo);

    if (ret != VK_SUCCESS)
    {
        return nullptr;
    }

    // 构造默认ImageView（常规2D/数组视图，可封装到Texture构造内部创建）
    RefPtr<Texture> tex = RefPtr<Texture>(new Texture(
        renderSystem,
        image, 
        allocation, 
        //allocResultInfo,
        desc.width, 
        desc.height, 
        desc.depth,
        desc.mipLevels, 
        desc.arrayLayers,
        desc.dimension, 
        desc.format));

    return tex;
}


Texture::Texture(
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
    TextureFormat fmt)
    : m_renderSystem(renderSystem)
    , m_image(image)
    , m_allocation(alloc)
    //, m_allocInfo(allocInfo)
    , m_width(width)
    , m_height(height)
    , m_depth(depth)
    , m_mipLevels(mips)
    , m_arrayLayers(layers)
    , m_dimension(dim)
    , m_format(fmt)
{
}

// 析构释放资源
Texture::~Texture()
{
    if (m_image != VK_NULL_HANDLE && m_renderSystem)
    {
        vmaDestroyImage(m_renderSystem->allocator(), m_image, m_allocation);
        m_image = VK_NULL_HANDLE;
        m_allocation = VK_NULL_HANDLE;
    }
}

END_GAIA_VK1