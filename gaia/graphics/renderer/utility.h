#pragma once
#include "../utility.h"

#if GAIA_PLATFORM == GAIA_PLATFORM_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#elif GAIA_PLATFORM == GAIA_PLATFORM_ANDROID
#define VK_USE_PLATFORM_ANDROID_KHR
#elif GAIA_PLATFORM == GAIA_PLATFORM_WAYLAND
#define VK_USE_PLATFORM_WAYLAND_KHR
#endif

#define BEGIN_GAIA namespace gaia { namespace graphics { namespace vk1 {
#define END_GAIA } }
