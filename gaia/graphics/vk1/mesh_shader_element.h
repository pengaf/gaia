#pragma once
#include "utility.h"

BEGIN_GAIA_VK1

class MeshShaderElement
{
public:
	MeshShaderElement() = default;
	~MeshShaderElement() = default;
	MeshShaderElement(const MeshShaderElement&) = delete;
	MeshShaderElement& operator=(const MeshShaderElement&) = delete;
	MeshShaderElement(MeshShaderElement&&) = default;
	MeshShaderElement& operator=(MeshShaderElement&&) = default;
};

END_GAIA_VK1