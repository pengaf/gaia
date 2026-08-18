#pragma once
#include "utility.h"
#include "vulkan.h"

BEGIN_GAIA_VK1_DETAIL

class RenderWindow;

class SingleThreadRenderScheduler
{
public:
	SingleThreadRenderScheduler();
	~SingleThreadRenderScheduler();
public:
	void render(const RenderWindow** renderWindows, uint32_t count);
};

END_GAIA_VK1_DETAIL
