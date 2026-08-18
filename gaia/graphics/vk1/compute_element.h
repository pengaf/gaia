#pragma once
#include "utility.h"

BEGIN_GAIA_GRAPHICS_VK1

class ComputeElement
{
public:
	ComputeElement() = default;
	~ComputeElement() = default;
	ComputeElement(const ComputeElement&) = delete;
	ComputeElement& operator=(const ComputeElement&) = delete;
	ComputeElement(ComputeElement&&) = default;
	ComputeElement& operator=(ComputeElement&&) = default;
};

END_GAIA_GRAPHICS_VK1