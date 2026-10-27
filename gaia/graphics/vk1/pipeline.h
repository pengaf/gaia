#pragma once
#include "utility.h"
#include "detail/vulkan.h"
#include <string>
#include <unordered_map>
#include "pafcore/hash.h"

BEGIN_GAIA_VK1

class GraphicsPipeline
{	
public:
	GraphicsPipeline();
	~GraphicsPipeline();
public:
	VkPipeline getPipeline(
		VkPipelineVertexInputStateCreateInfo* vertexInputState,
		VkPipelineInputAssemblyStateCreateInfo* inputAssemblyState,
		VkPipelineMultisampleStateCreateInfo* multisampleState,
		VkPipelineRenderingCreateInfo* rendering) const;
	VkPipelineLayout getPipelineLayout() const
	{
		return m_pipelineLayout;
	}
private:
	constexpr static uint32_t max_shader_stage_count = 5;
	VkShaderModule m_shaderModule{ VK_NULL_HANDLE };
	std::string m_vertexEntryPoint;
	std::string m_fragmentEntryPoint;
	std::string m_geometryEntryPoint;
	std::string m_hullEntryPoint;
	std::string m_domainEntryPoint;
	uint32_t m_stageCount;
	VkPipelineShaderStageCreateInfo m_shaderStages[max_shader_stage_count];
	VkPipelineTessellationStateCreateInfo m_tessellationState;
	VkPipelineRasterizationStateCreateInfo m_rasterizerState;
	VkPipelineDepthStencilStateCreateInfo m_depthStencilState;
	VkPipelineColorBlendStateCreateInfo m_colorBlendState;
	VkPipelineLayout m_pipelineLayout;
	struct Key
	{
		VkPipelineVertexInputStateCreateInfo* vertexInputState;
		VkPipelineInputAssemblyStateCreateInfo* inputAssemblyState;
		VkPipelineMultisampleStateCreateInfo* multisampleState;
		VkPipelineRenderingCreateInfo* rendering;
		bool operator==(const Key&) const = default;
	};
	struct KeyHash
	{
		size_t operator()(const Key& key) const
		{
			size_t hash = pafcore::fnv1a_64(key.vertexInputState);
			hash = pafcore::fnv1a_64(key.inputAssemblyState, hash);
			hash = pafcore::fnv1a_64(key.multisampleState, hash);
			hash = pafcore::fnv1a_64(key.rendering, hash);
			return hash;
		}
	};
	mutable std::unordered_map<Key, VkPipeline, KeyHash> m_pipelines;
};

class ComputePipeline
{
public:
	ComputePipeline();
	~ComputePipeline();
public:
	VkPipeline getPipeline() const
	{
		return m_pipeline;
	}
	VkPipelineLayout getPipelineLayout() const
	{
		return m_pipelineLayout;
	}
private:
	VkPipelineShaderStageCreateInfo m_stage;
	VkPipelineLayout m_pipelineLayout;
	VkPipeline m_pipeline;
};

END_GAIA_VK1