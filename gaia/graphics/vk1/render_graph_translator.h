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
	enum class DiagnosticCode
	{
		begin_error,
		error_create_texture_failed,
		error_create_buffer_failed,
		error_create_image_view_failed,
		end_error,
		begin_warning,
		warning_subresource_never_used,
		end_warning,
	};
	static constexpr uint32_t invalid_resource = UINT32_MAX;
	static constexpr uint32_t invalid_pass = UINT32_MAX;
	struct DiagnosticItem
	{
		DiagnosticCode code;
		uint32_t resource = invalid_resource;
		uint32_t pass = invalid_pass;
	};
	struct DiagnosticInfo
	{
		std::vector<DiagnosticItem> errors;
		std::vector<DiagnosticItem> warnings;
	};
public:
	static DiagnosticInfo translate(ExecutableRenderGraph* executableRenderGraph, const CompiledRenderGraph* compiledRenderGraph, RenderSystem* renderSystem);
};

END_GAIA_VK1