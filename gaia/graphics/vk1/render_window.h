#pragma once
#include "utility.h"
#include "detail/vulkan.h"
#include "../render_output.h"

BEGIN_GAIA_VK1

class RenderWindow : public RenderOutput
{
	friend class SingleThreadRenderScheduler;
public:
	static RenderWindow* New(void* platformHandle);
public:
	RenderWindow();
	~RenderWindow();
public:
	RenderWindow(const RenderWindow&) = delete;
	RenderWindow& operator=(const RenderWindow&) = delete;
	RenderWindow(RenderWindow&&) = default;
	RenderWindow& operator=(RenderWindow&&) = default;
public:
	virtual RenderTargetView* nextRenderTargetView() override;
private:
	VkSurfaceKHR m_surface;
	VkSwapchainKHR m_swapchain;
	VkCommandBuffer m_commandBuffers[gaia_max_swapchain_image_count];
};

END_GAIA_VK1
