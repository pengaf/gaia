#pragma once

#include "utility.h"
#include "../bit_mask_enum.h"
#include <string>

BEGIN_GAIA

const uint32_t gaia_max_bind_group_count = 4; //
const uint32_t gaia_max_color_attment_count = 8; //
const uint32_t gaia_max_vertex_buffer_bind_count = 8; //webgpu 8, d3d12 32, vulkan 16, metal 31
const uint32_t gaia_max_swapchain_image_count = 4;

enum class TextureFormat
{
    // 8 bit formats
    r8_unorm,
    r8_snorm,
    r8_uint,
    r8_sint,
    // 16 bit formats
    r16_unorm,
    r16_snorm,
    r16_uint,
    r16_sint,
    r16_float,
    rg8_unorm,
    rg8_snorm,
    rg8_uint,
    rg8_sint,
    // 32 bit formats
    r32_uint,
    r32_sint,
    r32_float,
    rg16_unorm,
    rg16_snorm,
    rg16_uint,
    rg16_sint,
    rg16_float,
    rgba8_unorm,
    rgba8_unorm_srgb,
    rgba8_snorm,
    rgba8_uint,
    rgba8_sint,
    bgra8_unorm,
    bgra8_unorm_srgb,
    // Packed 32 bit formats
    rgb9e5u_float,
    rgb10a2_uint,
    rgb10a2_unorm,
    rg11b10u_float,
    // 64 bit formats
    rg32_uint,
    rg32_sint,
    rg32_float,
    rgba16_unorm,
    rgba16_snorm,
    rgba16_uint,
    rgba16_sint,
    rgba16_float,
    // 128 bit formats
    rgba32_uint,
    rgba32_sint,
    rgba32_float,
    // Depth/stencil formats
    stencil8,
    depth16_unorm,
    depth24plus,
    depth24plus_stencil8,
    depth32_float,
    depth32_float_stencil8,
    // BC compressed formats
    bc1_rgba_unorm,
    bc1_rgba_unorm_srgb,
    bc2_rgba_unorm,
    bc2_rgba_unorm_srgb,
    bc3_rgba_unorm,
    bc3_rgba_unorm_srgb,
    bc4_r_unorm,
    bc4_r_snorm,
    bc5_rg_unorm,
    bc5_rg_snorm,
    bc6h_rgb_u_float,
    bc6h_rgb_float,
    bc7_rgba_unorm,
    bc7_rgba_unorm_srgb,
    // ETC2 compressed formats
    etc2_rgb8_unorm,
    etc2_rgb8_unorm_srgb,
    etc2_rgb8a1_unorm,
    etc2_rgb8a1_unorm_srgb,
    etc2_rgba8_unorm,
    etc2_rgba8_unorm_srgb,
    eac_r11_unorm,
    eac_r11_snorm,
    eac_rg11_unorm,
    eac_rg11_snorm,
    // ASTC compressed formats
    astc_4x4_unorm,
    astc_4x4_unorm_srgb,
    astc_5x4_unorm,
    astc_5x4_unorm_srgb,
    astc_5x5_unorm,
    astc_5x5_unorm_srgb,
    astc_6x5_unorm,
    astc_6x5_unorm_srgb,
    astc_6x6_unorm,
    astc_6x6_unorm_srgb,
    astc_8x5_unorm,
    astc_8x5_unorm_srgb,
    astc_8x6_unorm,
    astc_8x6_unorm_srgb,
    astc_8x8_unorm,
    astc_8x8_unorm_srgb,
    astc_10x5_unorm,
    astc_10x5_unorm_srgb,
    astc_10x6_unorm,
    astc_10x6_unorm_srgb,
    astc_10x8_unorm,
    astc_10x8_unorm_srgb,
    astc_10x10_unorm,
    astc_10x10_unorm_srgb,
    astc_12x10_unorm,
    astc_12x10_unorm_srgb,
    astc_12x12_unorm,
    astc_12x12_unorm_srgb,
    count,
    unknown = count,
};


enum class ShaderType
{
    vertex,
    fragment,
    geometry,
    hull,
    domain,
    compute,
    mesh,
    amplification,
    ray_generation,
    intersection,
    any_hit,
    closest_hit,
    miss,
    callable,
};

enum class CpuAccess
{
    none,
    read,
    write,
};

enum class BufferUsage
{
    copy_src = 0x0001,
    copy_dst = 0x0002,
    uniform = 0x0004,
    storage = 0x0008,
    index = 0x0010,
    vertex = 0x0020,
    indirect = 0x0040,
    query_resolve = 0x0080,
};

ENABLE_BITMASK(BufferUsage)

enum class TextureUsage
{
    copy_src = 0x0001,
    copy_dst = 0x0002,
    shader_resource = 0x0004,
    unordered_access = 0x0008,
    render_target = 0x0010,
    depth_stencil = 0x0020,
    input = 0x0040,
    transient = 0x0080,
};

ENABLE_BITMASK(TextureUsage)

enum class TextureDimension
{
    tex1d,
    tex2d,
    tex3d,
};

enum class PrimitiveTopology
{
	undefined,
	point_list,
	line_list,
	line_strip,
	triangle_list,
	triangle_strip,
	patch_list
};

enum class VertexInputRate
{
    vertex,
    instance,
};

enum class IndexType
{
    uint16,
    uint32,
};

enum class FillMode
{
    wireframe,
    solid
};

enum class CullMode
{
    none,
    front,
    back
};

enum class CompareFunc
{
    never,
    less,
    equal,
    less_equal,
    greater,
    not_equal,
    greater_equal,
    always
};

enum class StencilOp
{
    keep,
    zero,
    replace,
    incr_sat,
    decr_sat,
    invert,
    incr,
    decr
};

enum class BlendOp
{
    add,
    subtract,
    rev_subtract,
    min,
    max
};

enum class BlendFactor
{
    zero,
    one,
    src_color,
    inv_src_color,
    src_alpha,
    inv_src_alpha,
    dst_alpha,
    inv_dst_alpha,
    dst_color,
    inv_dst_color,
    src_alpha_saturate,
    blend_color,
    inv_blend_color
};

enum class RootParameterType
{
    descriptor_table,
    constants_32bit,
    constant_buffer_view,
    shader_resource_view,
    unordered_access_view
};

struct InputElementDesc
{
    const char* semanticName = nullptr;
    uint32_t semanticIndex = 0;
    uint32_t format = 0; // 这里通常对应底层的 DXGI_FORMAT 等枚举，用 uint32_t 兼容
    uint32_t inputSlot = 0;
    uint32_t alignedByteOffset = 0;
};

struct InputLayoutDesc
{
    InputElementDesc* elements = nullptr;
    uint32_t numElements = 0;
};

enum class SamplerAddressMode
{
    wrap,
    mirror,
    clamp,
    border
};

enum class FilterMode 
{
    point,
    linear
};

enum class MipmapMode
{
    nearest,
    linear
};

enum class BorderColor
{
    transparent_black,
    opaque_black,
    opaque_white,
    opaque_black_uint,
    opaque_white_uint
};

struct BufferDesc
{
    uint64_t size;
    BufferUsage bufferUsage;
    CpuAccess cpuAccess;
};

struct TextureDesc
{
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mipLevels;
    uint32_t arrayLayers;
    TextureDimension dimension;
    TextureFormat format;
    TextureUsage textureUsage;
    CpuAccess cpuAccess;
};

struct SamplerState 
{
    SamplerAddressMode addressU = SamplerAddressMode::wrap;
    SamplerAddressMode addressV = SamplerAddressMode::wrap;
    SamplerAddressMode addressW = SamplerAddressMode::wrap;
    FilterMode magFilter = FilterMode::linear;
	FilterMode minFilter = FilterMode::linear;
    MipmapMode mipmapMode = MipmapMode::linear;
    float mipLodBias = 0.0f;
    bool anisotropyEnable = false;
    float maxAnisotropy = 1.0f;
    bool compareEnable = false;
    CompareFunc compareFunc = CompareFunc::less;
    float minLod = 0.0f;
    float maxLod = FLT_MAX;
    BorderColor borderColor = BorderColor::transparent_black;
};

struct SampleDesc 
{
    uint32_t count = 1;
    uint32_t quality = 0;
};

struct RasterizerStateDesc
{
    FillMode fillMode = FillMode::solid;
    CullMode cullMode = CullMode::back;
    bool frontCounterClockwise = false;
    int32_t depthBias = 0;
    float depthBiasClamp = 0.0f;
    float slopeScaledDepthBias = 0.0f;
    bool depthClipEnable = true;
    bool multisampleEnable = false;
    bool antialiasedLineEnable = false;
};

struct DepthStencilDesc
{
    bool depthEnable = true;
    bool depthWriteMask = true;
    CompareFunc depthFunc = CompareFunc::less;

    bool stencilEnable = false;
    uint8_t stencilReadMask = 0xFF;
    uint8_t stencilWriteMask = 0xFF;

    StencilOp frontFaceStencilFail = StencilOp::keep;
    StencilOp frontFaceStencilDepthFail = StencilOp::keep;
    StencilOp frontFaceStencilPass = StencilOp::keep;
    CompareFunc frontFaceStencilFunc = CompareFunc::always;

    StencilOp backFaceStencilFail = StencilOp::keep;
    StencilOp backFaceStencilDepthFail = StencilOp::keep;
    StencilOp backFaceStencilPass = StencilOp::keep;
    CompareFunc backFaceStencilFunc = CompareFunc::always;
};

struct RenderTargetBlendDesc
{
    bool blendEnable = false;
    BlendFactor srcBlend = BlendFactor::one;
    BlendFactor dstBlend = BlendFactor::zero;
    BlendOp blendOp = BlendOp::add;
    BlendFactor srcBlendAlpha = BlendFactor::one;
    BlendFactor dstBlendAlpha = BlendFactor::zero;
    BlendOp blendOpAlpha = BlendOp::add;
    uint8_t renderTargetWriteMask = 0xF;
};

struct BlendStateDesc
{
    bool alphaToCoverageEnable = false;
    bool independentBlendEnable = false;
    RenderTargetBlendDesc renderTargets[8] = {};
};

struct GraphicsPipelineDesc
{
	std::string shaderCode;
	std::string vertexEntryPoint;
	std::string fragmentEntryPoint;
	std::string geometryEntryPoint;
	std::string hullEntryPoint;
	std::string domainEntryPoint;
    RasterizerStateDesc rasterizerState;
    DepthStencilDesc depthStencilState;
    BlendStateDesc blendState;
}; 

struct ComputePipelineDesc
{
	std::string shaderCode;
	std::string computeEntryPoint;
};

struct RayTracingPipelineDesc
{
	std::string shaderCode;
	std::string rayGenerationEntryPoint;
	std::string missEntryPoint;
	std::string closestHitEntryPoint;
	std::string anyHitEntryPoint;
	std::string intersectionEntryPoint;
};

struct MeshShaderPipelineDesc
{
	std::string shaderCode;
	std::string meshEntryPoint;
	std::string amplificationEntryPoint;
    RasterizerStateDesc rasterizerState;
    DepthStencilDesc depthStencilState;
    BlendStateDesc blendState;
};


enum class DrawMethod
{
    undefined,
    draw,
    draw_indexed,
    draw_indirect,
    draw_indexed_indirect,
    //draw_indirect_count,
    //draw_indexed_indirect_count,
};

struct Draw
{
   uint32_t vertexCount;
   uint32_t instanceCount;
   uint32_t firstVertex;
   uint32_t firstInstance;
};

struct DrawIndexed
{
    uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t vertexOffset;
    uint32_t firstInstance;
};

struct DrawIndirect
{
    uint32_t offset;
    uint32_t drawCount;
    uint32_t stride;
};

struct DrawIndexedIndirect
{
    uint32_t offset;
    uint32_t drawCount;
    uint32_t stride;
};

struct DrawIndirectCount
{
    uint32_t offset;
    uint32_t countBufferOffset;
    uint32_t maxDrawCount;
    uint32_t stride;
};

struct DrawIndexedIndirectCount
{
    uint32_t offset;
    uint32_t countBufferOffset;
    uint32_t maxDrawCount;
    uint32_t stride;
};

struct DrawCall
{
    PrimitiveTopology primitiveTopology;
    DrawMethod drawMethod;
    union
    {
        Draw draw;
        DrawIndexed drawIndexed;
        DrawIndirect drawIndirect;
        DrawIndexedIndirect drawIndexedIndirect;
		//DrawIndirectCount drawIndirectCount;
		//DrawIndexedIndirectCount drawIndexedIndirectCount;
    };
};

enum class RenderPassKind
{
    scene_pass,
    image_pass,
    compute_pass,
    ray_tracing_pass,
};

END_GAIA

