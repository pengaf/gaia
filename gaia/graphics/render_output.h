#pragma once

#include "utility.h"

BEGIN_GAIA_GRAPHICS

class RenderTargetView;
class Pass;

class RenderOutput
{
public:
	virtual ~RenderOutput() = default;
	virtual RenderTargetView* nextRenderTargetView() = 0;
protected:
	RefPtr<Pass> m_pass;
};

END_GAIA_GRAPHICS
