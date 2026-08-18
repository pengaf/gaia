#pragma once

#include "utility.h"
#include "../math/axis_aligned_box.h"

BEGIN_GAIA_GRAPHICS

class RenderSystem;
class Mesh;
class Material;

class Renderable
{
public:
	virtual ~Renderable() = default;
	virtual void build(RenderSystem* renderSystem) = 0;
protected:
	std::vector<VertexShaderElement> m_graphicsElements;
};

END_GAIA_GRAPHICS
