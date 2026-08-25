#pragma once

#include "hal/vertex_shader_element.h"
#include <vector>

BEGIN_GAIA

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

END_GAIA
