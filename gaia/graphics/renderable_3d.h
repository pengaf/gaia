#pragma once

#include "renderable.h"
#include "../math/matrix.h"

BEGIN_GAIA_GRAPHICS

using math::AffineTransform3D;

class Renderable3D : public Renderable
{
public:
	void setWorldTransform(const AffineTransform3D& worldTransform);
protected:
	AffineTransform3D m_worldTransform;
};

END_GAIA_GRAPHICS