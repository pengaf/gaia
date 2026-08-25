#pragma once
#include "utility.h"
#include "detail/vulkan.h"

BEGIN_GAIA_VK1

class GraphicsPipeline
{	
public:
	GraphicsPipeline();
	~GraphicsPipeline();
private:
	VkShaderModule m_shaderModule{ VK_NULL_HANDLE };
	std::string m_vertexEntryPoint;
	std::string m_fragmentEntryPoint;
	std::string m_geometryEntryPoint;
	std::string m_hullEntryPoint;
	std::string m_domainEntryPoint;
	VkPipelineRasterizationStateCreateInfo m_rasterizerState{};
};

END_GAIA_VK1