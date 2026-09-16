#pragma once
#include "utility.h"

BEGIN_GAIA

class RenderGraphCompiler;
class GraphicsPipelineState;
class ComputePipelineState;
class RayTracingPipelineState;

class RenderPass
{
    friend class RenderGraphCompiler;
public:
    RenderPassKind kind() const
    {
        return m_kind;
    }
protected:
    RenderPassKind m_kind;
    const char* m_name;
};

class ScenePass : public RenderPass
{
    friend class RenderGraphCompiler;
public:
    void addRTV(uint32_t rtv);
    void setDSV(uint32_t dsv);
protected:
    std::vector<uint32_t> m_rtvs;
    uint32_t m_dsv;
};

class ImagePass : public RenderPass
{
    friend class RenderGraphCompiler;
public:
    void addSRV(uint32_t srv);
    void addRTV(uint32_t rtv);
    void setPipeline(GraphicsPipelineState* pipeline);
protected:
    std::vector<uint32_t> m_srvs;
    std::vector<uint32_t> m_rtvs;
    GraphicsPipelineState* m_pipeline;
};

class ComputePass : public RenderPass
{
    friend class RenderGraphCompiler;
public:
    void addSRV(uint32_t srv);
    void addUAV(uint32_t uav);
    void setPipeline(ComputePipelineState* pipeline);
protected:
    std::vector<uint32_t> m_srvs;
    std::vector<uint32_t> m_uavs;
    ComputePipelineState* m_pipeline;
};

class RayTracingPass : public RenderPass
{
    friend class RenderGraphCompiler;
public:
    void addSRV(uint32_t srv);
    void addUAV(uint32_t uav);
    void setPipeline(RayTracingPipelineState* pipeline);
protected:
    std::vector<uint32_t> m_srvs;
    std::vector<uint32_t> m_uavs;
    RayTracingPipelineState* m_pipeline;
};

END_GAIA