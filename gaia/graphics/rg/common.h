#pragma once
#include "utility.h"
#include "../common.h"

BEGIN_GAIA_RG

struct TextureHandle
{
	uint32_t index;
};

struct BufferHandle
{
	uint32_t index;
};


struct TextureDesc
{
    uint32_t width = 1;
    uint32_t height = 1;
    uint32_t depth = 1;
    uint32_t arrayLayers = 1;
    uint32_t mipLevels = 1;
    uint8_t sampleCount = 1;

    TextureFormat format = TextureFormat::unknown;
    uint32_t usage = 0;

    uint8_t tiling = 0;
    uint8_t memoryType = 0;

    const char* name;
}

END_GAIA_RG
