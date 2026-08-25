#pragma once
#include "utility.h"
#include "graphics/mesh.h"
#include "pafcore/object.h"
#include "pafcore/memory.h"

BEGIN_GAIA

class MeshSource : public pafcore::Object
{
public:
	virtual RefPtr<Mesh> getMesh() = 0;
private:
//	RefPtr<Mesh> m_mesh;
};

END_GAIA
