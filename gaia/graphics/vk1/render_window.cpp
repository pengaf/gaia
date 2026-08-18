#include "render_window.h"

BEGIN_GAIA_GRAPHICS_VK1

RenderWindow::RenderWindow()
{
	for (uint32_t i = 0; i < gaia_max_swapchain_image_count; ++i)
	{
		m_commandBuffers[i] = VK_NULL_HANDLE;
	}
}

RenderTargetView* RenderWindow::nextRenderTargetView()
{
	return nullptr;
}

END_GAIA_GRAPHICS_VK1