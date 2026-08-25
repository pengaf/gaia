#pragma once
#include "utility.h"
#include "../vk1/hal_fwd.h"

BEGIN_GAIA

using Buffer = vk1::Buffer;
using Texture = vk1::Texture;
using ShaderResourceView = vk1::ShaderResourceView;
using UnorderedAccessView = vk1::UnorderedAccessView;
using RenderTargetView = vk1::RenderTargetView;
using DepthStencilView = vk1::DepthStencilView;
using Sampler = vk1::Sampler;

using GraphicsState = vk1::GraphicsState;
using ComputeState = vk1::ComputeState;
using RayTracingState = vk1::RayTracingState;
using MeshShaderState = vk1::MeshShaderState;

using GraphicsPipelineState = vk1::GraphicsPipelineState;
using ComputePipelineState = vk1::ComputePipelineState;
using RayTracingPipelineState = vk1::RayTracingPipelineState;
using MeshShaderPipelineState = vk1::MeshShaderPipelineState;
using RenderWindow = vk1::RenderWindow;

using RenderElement = vk1::RenderElement;
using VertexShaderElement = vk1::VertexShaderElement;
using ComputeElement = vk1::ComputeElement;
using RayTracingElement = vk1::RayTracingElement;
using MeshShaderElement = vk1::MeshShaderElement;

END_GAIA
