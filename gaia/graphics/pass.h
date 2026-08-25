#pragma once
#include "utility.h"
#include "hal/hal_fwd.h"
#include <vector>

BEGIN_GAIA

enum class PassType
{
	serial,
	parallel,
	graphics,
	compute,
};

class Pass
{};


class RenderPassBase : public Pass
{
public:
	void addRenderElement(RenderElement* element);
	void addRenderElementSequence(RenderElement** elements, size_t count);
protected:
	std::vector<RenderElement*> m_elements;
	std::vector<RenderElement*> m_subElements;
};


class ComputePassBase : public Pass
{
public:
	void addComputeElement(ComputeElement* element);
protected:
	std::vector<ComputeElement*> m_elements;
};


class RayTracingPassBase : public Pass
{
public:
	void addRayTracingElement(RayTracingElement* element);
protected:
	std::vector<RayTracingElement*> m_elements;
};


END_GAIA
