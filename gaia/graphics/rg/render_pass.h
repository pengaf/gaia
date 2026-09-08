#pragma once

#include "utility.h"
#include "common.h"
#include "../hal/hal_fwd.h"
#include <vector>

BEGIN_GAIA_RG

class RenderGraphBuilder;

class RenderPass
{
    friend class RenderGraphBuilder;
private:
    const char* m_name;
};

class ScenePass : public RenderPass
{
public:
    void addRTV(TextureHandle rtv);
    void setDSV(TextureHandle dsv);
private:
    std::vector<TextureHandle> m_rtvs;
    TextureHandle m_dsv;
};

class ImagePass : public RenderPass
{
public:
    void addSRV(TextureHandle srv);
    void addRTV(TextureHandle rtv);
    void setPipeline(GraphicsPipelineState* pipeline);
private:
    std::vector<TextureHandle> m_srvs;
    std::vector<TextureHandle> m_rtvs;
    GraphicsPipelineState* m_pipeline;
};

class ComputePass : public RenderPass
{
public:
    void addSRV(TextureHandle srv);
    void addUAV(TextureHandle uav);
    void setPipeline(ComputePipelineState* pipeline);
private:
    std::vector<TextureHandle> m_srvs;
    std::vector<TextureHandle> m_uavs;
    ComputePipelineState* m_pipeline;
};

class RayTracingPass : public RenderPass
{
public:
    void addSRV(TextureHandle srv);
    void addUAV(TextureHandle uav);
    void setPipeline(RayTracingPipelineState* pipeline);
private:
    std::vector<TextureHandle> m_srvs;
    std::vector<TextureHandle> m_uavs;
    RayTracingPipelineState* m_pipeline;
};

END_GAIA_RG
