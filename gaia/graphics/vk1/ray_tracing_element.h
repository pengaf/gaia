#pragma once
#include "utility.h"

BEGIN_GAIA_GRAPHICS_VK1

class RayTracingElement
{
public:
	RayTracingElement() = default;
	~RayTracingElement() = default;
	RayTracingElement(const RayTracingElement&) = delete;
	RayTracingElement& operator=(const RayTracingElement&) = delete;
	RayTracingElement(RayTracingElement&&) = default;
	RayTracingElement& operator=(RayTracingElement&&) = default;
};

END_GAIA_GRAPHICS_VK1
