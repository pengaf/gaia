#pragma once

#include "utility.h"
#include "common.h"

BEGIN_GAIA_RG

struct RenderResource
{
    const char* m_name;
};

struct RenderTexture : public RenderResource
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
};

struct RenderBuffer : public RenderResource
{
	uint64_t size = 0;
	uint32_t usage = 0;
	uint8_t memoryType = 0;
};


END_GAIA_RG
