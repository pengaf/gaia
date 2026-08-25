#pragma once

#include "renderable_3d.h"
#include <vector>

BEGIN_GAIA

class StaticMeshRenderable3D : public Renderable3D
{
public:
	virtual void build(RenderSystem* renderSystem) override;
protected:
	RefPtr<Mesh> m_mesh;
	std::vector<RefPtr<Material>> m_material;
};

END_GAIA
