#pragma once
#include "utility.h"

BEGIN_GAIA

enum class RenderElementStage : uint8_t
{
	depth_prepass = 10,
	shadow_map = 30,
	light_culling = 50,
	opaque = 70,
	cutout = 90,
	decal = 110,
	transparent = 130,
	transparent_decal = 150,
	post_process = 170,
	world_space_ui = 190,
	screen_space_ui = 210,
};

enum class RenderElementType : uint8_t
{
	vertex_shader_element,
	computer_element,
	mesh_shader_element,
	ray_tracing_element,
	render_element_group,
};

END_GAIA
