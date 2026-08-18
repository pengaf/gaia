#pragma once
#include "utility.h"
#include "../common.h"
#include "detail/vulkan.h"
#include "detail/vk_mem_alloc.h"

BEGIN_GAIA_GRAPHICS_VK1

enum class ViewType
{
    image_view,
    buffer_view,
    buffer_range,
};

struct BufferRangeView
{
	RefPtr<Buffer> m_buffer;
    VkDeviceSize m_offset{ 0 };
    VkDeviceSize m_range{ VK_WHOLE_SIZE };
};

struct ImageView
{
	RefPtr<Texture> m_texture;
	VkImageViewType m_viewType{ VK_IMAGE_VIEW_TYPE_2D };
	VkFormat m_format{ VK_FORMAT_UNDEFINED };
	VkImageSubresourceRange m_subresourceRange{};
};

struct BufferView
{
	RefPtr<Buffer> m_buffer;
	VkFormat m_format{ VK_FORMAT_UNDEFINED };
	VkDeviceSize m_offset{ 0 };
	VkDeviceSize m_range{ VK_WHOLE_SIZE };
};

class SrvUav
{
    union
	{
		ImageView m_imageView;
		BufferView m_bufferView;
		BufferRangeView bufferRangeView;
    };
};


class RtvDsv
{
protected:
    RefPtr<Texture> m_texture;
    VkImageView m_imageView;
};

class ShaderResourceView ： public SrvUav
{
};

class UnorderedAccessView ： public SrvUav
{
};

class RenderTargetView : public RtvDsv
{
public:
	RefPtr<RenderTargetView> New(RenderSystem* renderSystem, Texture* texture, const RenderTargetViewDesc& viewDesc);
};

class DepthStencilView : public RtvDsv
{
public:
	RefPtr<RenderTargetView> New(RenderSystem* renderSystem, Texture* texture, const RenderTargetViewDesc& viewDesc);
};

END_GAIA_GRAPHICS_VK1