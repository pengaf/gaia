#pragma once

#include "renderable.h"
#include "../math/matrix.h"

BEGIN_GAIA

class Renderable3D : public Renderable
{
public:
	void setWorldTransform(const AffineTransform3D& worldTransform);
protected:
	AffineTransform3D m_worldTransform;
};

END_GAIA