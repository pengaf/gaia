#pragma once
#include "utility.h"

BEGIN_GAIA_GRAPHICS_VK1

class RenderSystem;

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

class ShaderBase
{
	friend class RenderSystem;
protected:
	ShaderBase(const char* code, size_t codeSize, const char* entryPoint);
	ShaderBase(const Shader&) = delete;
protected:
	VkShaderModule m_shaderModule;
	VkDescriptorSetLayout m_descriptorSetLayout;
	const char* m_entryPoint;
}

template<ShaderType t_shaderType>
class Shader : public ShaderBase
{
	friend class RenderSystem;
private:
	using ShaderBase::ShaderBase;
};


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


END_GAIA_GRAPHICS_VK1