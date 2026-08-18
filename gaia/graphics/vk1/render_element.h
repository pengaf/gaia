#pragma once
#include "utility.h"

BEGIN_GAIA_GRAPHICS_VK1

enum class RenderElementType : uint8_t
{
	vertex_shader_element,
	mesh_shader_element,
};

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

END_GAIA_GRAPHICS_VK1
