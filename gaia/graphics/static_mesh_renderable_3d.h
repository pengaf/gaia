#pragma once

#include "render_able_3d.h"
#include "graphics_element.h"

BEGIN_GAIA_GRAPHICS

class StaticMeshRenderable3D : public Renderable3D
{
public:
	virtual void build(RenderSystem* renderSystem) override;
protected:
	RefPtr<Mesh> m_mesh;
	std::vector<RefPtr<Material>> m_material;
};

END_GAIA_GRAPHICS
