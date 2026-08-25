#pragma once
#include <cstdint>
#include <cfloat>
#include "pafcore/utility.h"

#define BEGIN_GAIA namespace gaia {
#define END_GAIA }

#ifndef GAIA_PLATFORM_WEB
#define GAIA_PLATFORM_WEB				1
#endif

#ifndef GAIA_PLATFORM_WIN32
#define GAIA_PLATFORM_WIN32				2
#endif

#ifndef GAIA_PLATFORM_ANDROID
#define GAIA_PLATFORM_ANDROID			3
#endif

#ifndef GAIA_PLATFORM_WAYLAND
#define GAIA_PLATFORM_WAYLAND			4
#endif

#ifndef GAIA_PLATFORM_XCB
#define GAIA_PLATFORM_XCB				5
#endif

#ifndef GAIA_PLATFORM
#define GAIA_PLATFORM					GAIA_PLATFORM_WIN32
#endif

#define GAIA_HIGH_PRECISION_POSITION_2D		0
#define GAIA_HIGH_PRECISION_POSITION_3D		0

#define GAIA_ASSERT PAF_ASSERT
