#pragma once
#include "utility.h"
#include "../common.h"

BEGIN_GAIA_VK1

class RenderSystem;

class ShaderBase
{
	friend class RenderSystem;
protected:
	ShaderBase(const char* code, size_t codeSize, const char* entryPoint);
	ShaderBase(const ShaderBase&) = delete;
protected:
	VkShaderModule m_shaderModule;
	VkDescriptorSetLayout m_descriptorSetLayout;
	const char* m_entryPoint;
};

template<ShaderType t_shaderType>
class Shader : public ShaderBase
{
	friend class RenderSystem;
private:
	using ShaderBase::ShaderBase;
};


END_GAIA_VK1