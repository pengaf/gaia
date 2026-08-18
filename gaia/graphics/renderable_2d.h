#pragma once

#include "renderable.h"
#include "../math/matrix.h"

BEGIN_GAIA_GRAPHICS

using math::AffineTransform2D;

class Renderable2D : public Renderable
{
protected:
	AffineTransform2D m_worldTransform;
};

END_GAIA_GRAPHICS