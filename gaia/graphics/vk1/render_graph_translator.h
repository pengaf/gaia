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
		error_invalid_texture,
		error_invalid_buffer,
		error_resource_is_not_texture,
		error_invalid_rtv_format,
		error_invalid_dsv_format,

		error_create_texture_failed,
		error_create_buffer_failed,
		error_create_image_view_failed,

		error_scene_pass_too_many_rtvs,
		error_scene_pass_no_output,
		error_scene_pass_invalid_render_size,

		error_image_pass_too_many_rtvs,
		error_image_pass_no_output,
		error_image_pass_invalid_render_size,

		error_invalid_graphics_pipeline,
		error_invalid_compute_pipeline,
		error_invalid_ray_tracing_pipeline,

		end_error,
		begin_warning,
		warning_subresource_never_used,
		end_warning,
	};
	static constexpr uint32_t invalid_pass = UINT32_MAX;
	static constexpr uint32_t invalid_resource = UINT32_MAX;
	struct DiagnosticItem
	{
		DiagnosticCode code;
		uint32_t pass = invalid_pass;
		uint32_t resource = invalid_resource;
	};
	struct DiagnosticInfo
	{
		std::vector<DiagnosticItem> errors;
		std::vector<DiagnosticItem> warnings;
	public:
		void addError(const DiagnosticItem& item)
		{
			GAIA_ASSERT(DiagnosticCode::begin_error < item.code && item.code < DiagnosticCode::end_error);
			errors.push_back(item);
		}
		void addWarning(const DiagnosticItem& item)
		{
			GAIA_ASSERT(DiagnosticCode::begin_warning < item.code && item.code < DiagnosticCode::end_warning);
			warnings.push_back(item);
		}
	};
public:
	static DiagnosticInfo translate(ExecutableRenderGraph* executableRenderGraph, const CompiledRenderGraph* compiledRenderGraph, RenderSystem* renderSystem);
};

END_GAIA_VK1