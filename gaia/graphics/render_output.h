#pragma once

#include "utility.h"
#include "hal/hal_fwd.h"
#include "pass.h"

BEGIN_GAIA

//class Pass;

class RenderOutput
{
public:
	virtual ~RenderOutput() = default;
	virtual RenderTargetView* nextRenderTargetView() = 0;
protected:
	RefPtr<Pass> m_pass;
};

END_GAIA
