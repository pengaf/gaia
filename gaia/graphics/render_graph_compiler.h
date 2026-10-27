#pragma once

#include "utility.h"
#include <vector>

BEGIN_GAIA

class RenderGraph;
class CompiledRenderGraph;

class RenderGraphCompiler
{
public:
	enum DiagnosticCode
	{
		begin_error,
		error_external_resource_null,
		error_duplicate_external_resource,
		error_resource_out_of_range,
		error_resource_read_before_write,
		error_duplicate_resource_in_pass,
		error_resource_binding_mismatch,
		error_output_resource_not_marked,
		error_output_resource_out_of_range,
		error_output_resource_not_written,
		error_scene_pass_no_output,
		error_image_pass_no_pipeline,
		error_image_pass_no_rtv,
		error_compute_pass_no_pipeline,
		error_compute_pass_no_uav,
		error_ray_tracing_pass_no_pipeline,
		error_ray_tracing_pass_no_uav,
		end_error,
		begin_warning,
		warning_resource_not_written,
		warning_resource_not_read,
		warning_pass_no_contribution,
		warning_attachment_dimension_mismatch,
		//warning_duplicate_resource_name,
		//warning_duplicate_pass_name,
		warning_image_pass_no_srv,
		warning_compute_pass_no_srv,
		warning_ray_tracing_pass_no_srv,
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
			GAIA_ASSERT(begin_error < item.code && item.code < end_error);
			errors.push_back(item);
		}
		void addWarning(const DiagnosticItem& item)
		{
			GAIA_ASSERT(begin_warning < item.code && item.code < end_warning);
			warnings.push_back(item);
		}
	};
public:
	static DiagnosticInfo compile(CompiledRenderGraph* compiledRenderGraph, const RenderGraph* renderGraph);
};

END_GAIA
