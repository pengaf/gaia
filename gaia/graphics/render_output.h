#pragma once

#include "utility.h"
#include "hal/hal_fwd.h"
#include "render_pass.h"
#include <vector>

BEGIN_GAIA

//class Pass;

class RenderOutput
{
public:
	virtual ~RenderOutput() = default;
	virtual RenderTargetView* nextRenderTargetView() = 0;
protected:
	std::vector<RenderPass> m_renderPasses;
};

END_GAIA
