#pragma once
#include "utility.h"
#include "../bit_mask_enum.h"

BEGIN_GAIA_GRAPHICS

const uint32_t gaia_max_vertex_buffer_bind_count = 8; //webgpu 8, d3d12 32, vulkan 16, metal 31
const uint32_t gaia_max_swapchain_image_count = 4;

enum class CpuAccess
{
    indirect_write,
    direct_write,
    direct_read,
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
}；

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
}；

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

typedef enum BorderColor
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

END_GAIA_RHI

