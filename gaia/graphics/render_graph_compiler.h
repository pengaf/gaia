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
		error_resource_out_of_range,
		error_resource_read_before_write,
		error_duplicate_external_resource,
		end_error,
		begin_warning,
		warning_resource_not_written,
		warning_resource_not_read,
		warning_pass_no_contribution,
		end_warning,
	};
	struct DiagnosticItem
	{
		DiagnosticCode code;
		uint32_t resource = UINT32_MAX;
		uint32_t pass = UINT32_MAX;
	};
	struct DiagnosticInfo
	{
		std::vector<DiagnosticItem> errors;
		std::vector<DiagnosticItem> warnings;
	};
public:
	DiagnosticInfo compile(CompiledRenderGraph* compiledRenderGraph, const RenderGraph* renderGraph);
};

END_GAIA
