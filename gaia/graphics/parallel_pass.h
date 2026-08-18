#pragma once
#include "pass.h"
#include <vector>

BEGIN_GAIA_GRAPHICS

class ParallelPass : public Pass
{
public:
	ParallelPass();
	void addPass(const Pass* pass);
private:
	std::vector<Pass*> m_passes;
};

END_GAIA_GRAPHICS
