#pragma once
#include "utility.h"
#include "../common.h"

BEGIN_GAIA_VK1

class Buffer;
class Texture;
class ShaderResourceView;
class UnorderedAccessView;
class RenderTargetView;
class DepthStencilView;
class Sampler;

class GraphicsState;
class ComputeState;
class RayTracingState;
class MeshShaderState;

class GraphicsPipelineState;
class ComputePipelineState;
class RayTracingPipelineState;
class MeshShaderPipelineState;

class VertexShaderElement;
class MeshShaderElement;
class RayTracingElement;
class RenderWindow;

class RenderElement;
class VertexShaderElement;
class ComputeElement;
class RayTracingElement;
class MeshShaderElement;


template<ShaderType t_shaderType>
class Shader;
using VertexShader = Shader<ShaderType::vertex>;
using FragmentShader = Shader<ShaderType::fragment>;
using GeometryShader = Shader<ShaderType::geometry>;
using HullShader = Shader<ShaderType::hull>;
using DomainShader = Shader<ShaderType::domain>;
using ComputeShader = Shader<ShaderType::compute>;
using MeshShader = Shader<ShaderType::mesh>;
using AmplificationShader = Shader<ShaderType::amplification>;
using RayGenerationShader = Shader<ShaderType::ray_generation>;
using IntersectionShader = Shader<ShaderType::intersection>;
using AnyHitShader = Shader<ShaderType::any_hit>;
using ClosestHitShader = Shader<ShaderType::closest_hit>;
using MissShader = Shader<ShaderType::miss>;
using CallableShader = Shader<ShaderType::callable>;


END_GAIA_VK1
