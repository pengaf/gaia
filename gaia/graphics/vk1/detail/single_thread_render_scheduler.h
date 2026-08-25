#pragma once
#include "utility.h"
#include "vulkan.h"

BEGIN_GAIA_VK1

class RenderWindow;

class SingleThreadRenderScheduler
{
public:
	SingleThreadRenderScheduler();
	~SingleThreadRenderScheduler();
public:
	void render(RenderWindow** renderWindows, uint32_t count);
};

END_GAIA_VK1
