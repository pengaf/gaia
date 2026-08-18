#pragma once

#include "render_able_2d.h"

BEGIN_GAIA_GRAPHICS

class StaticMeshRenderable2D : public Renderable2D
{
public:
	virtual void build(RenderSystem* renderSystem) override;
protected:
	RefPtr<Mesh> m_mesh;
	std::vector<RefPtr<Material>> m_material;
};

END_GAIA_GRAPHICS