#pragma once
#include "utility.h"
#include "../compiled_render_graph.h"
#include "executable_render_graph.h"

BEGIN_GAIA_VK1

class RenderSystem;
class ExecutableRenderGraph;

class RenderGraphTranslator
{
public:
	enum class ErrorCode
	{
		success,
		create_texture_failed,
		create_buffer_failed,
		create_image_view_failed,
	};
	static constexpr uint32_t invalid_resource = UINT32_MAX;
	static constexpr uint32_t invalid_pass = UINT32_MAX;
	struct ErrorItem
	{
		ErrorCode code;
		uint32_t resource = invalid_resource;
		uint32_t pass = invalid_pass;
	};
	struct ErrorInfo
	{
		std::vector<ErrorItem> errors;
	};
public:
	static ErrorInfo translate(ExecutableRenderGraph* executableRenderGraph, const CompiledRenderGraph* compiledRenderGraph, RenderSystem* renderSystem);
};

END_GAIA_VK1