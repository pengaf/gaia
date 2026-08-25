#pragma once
#include "utility.h"

BEGIN_GAIA

//class GraphicsRenderState 
//{
//public:
//    void setVertexShader(Shader* vs) { m_vs = vs; }
//    void setHullShader(Shader* hs) { m_hs = hs; }
//    void setDomainShader(Shader* ds) { m_ds = ds; }
//    void setGeometryShader(Shader* gs) { m_gs = gs; }
//    void setPixelShader(Shader* ps) { m_ps = ps; }
//
//    void setBlendState(BlendState* state) { m_blend = state; }
//    void setDepthStencilState(DepthStencilState* state) { m_depthStencil = state; }
//    void setRasterizerState(RasterizerState* state) { m_rasterizer = state; }
//
//private:
//    Shader* m_vs = nullptr;
//    Shader* m_hs = nullptr;
//    Shader* m_ds = nullptr;
//    Shader* m_gs = nullptr;
//    Shader* m_ps = nullptr;
//
//    BlendState* m_blend = nullptr;
//    DepthStencilState* m_depthStencil = nullptr;
//    RasterizerState* m_rasterizer = nullptr;
//};
//
//class MeshRenderState 
//{
//public:
//    void setTaskShader(Shader* ts) { m_ts = ts; }      // 可选：Task Shader
//    void setMeshShader(Shader* ms) { m_ms = ms; }
//    void setPixelShader(Shader* ps) { m_ps = ps; }
//
//    void setBlendState(BlendState* state) { m_blend = state; }
//    void setDepthStencilState(DepthStencilState* state) { m_depthStencil = state; }
//    void setRasterizerState(RasterizerState* state) { m_rasterizer = state; }
//
//
//private:
//    Shader* m_ts = nullptr;   // Task Shader（Amplification）
//    Shader* m_ms = nullptr;   // Mesh Shader
//    Shader* m_ps = nullptr;   // Pixel Shader
//
//    BlendState* m_blend = nullptr;
//    DepthStencilState* m_depthStencil = nullptr;
//    RasterizerState* m_rasterizer = nullptr;
//};
//
//
//class RayTracingRenderState 
//{
//public:
//    void setRayGenShader(Shader* rgs) { m_rgs = rgs; }
//    void setMissShader(Shader* ms) { m_miss = ms; }
//    void setClosestHitShader(Shader* chs) { m_chs = chs; }
//
//    void setAnyHitShader(Shader* ahs) { m_ahs = ahs; }
//    void setIntersectionShader(Shader* is) { m_is = is; }  // 程序化几何体
//
//    // 光线参数
//    void setMaxRecursionDepth(uint32_t depth) { m_maxRecursionDepth = depth; }
//
//private:
//    Shader* m_rgs = nullptr;   // Ray Generation
//    Shader* m_miss = nullptr;  // Miss
//    Shader* m_chs = nullptr;   // Closest Hit
//    Shader* m_ahs = nullptr;   // Any Hit（可选）
//    Shader* m_is = nullptr;    // Intersection（可选）
//
//    uint32_t m_maxRecursionDepth = 1;
//};
//

END_GAIA
