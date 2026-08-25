#pragma once
#include "utility.h"
#include "../hal/render_element_common.h"

BEGIN_GAIA_VK1

class RenderElement
{
	RenderElement(const RenderElement&) = delete;
	RenderElement& operator=(const RenderElement&) = delete;
	RenderElement(RenderElement&&) = delete;
	RenderElement& operator=(RenderElement&&) = delete;
protected:
	RenderElement(RenderElementType type) : m_type(type)
	{}
	virtual ~RenderElement() = default;
	friend class RenderPassBase;
protected:
	RenderElementType m_type;
	int8_t m_priority{ 0 };
	uint8_t m_subCount{ 0 };
	uint32_t m_subOffset{ 0 };
};

END_GAIA_VK1
