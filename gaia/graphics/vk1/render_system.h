#pragma once
#include "utility.h"
#include "detail/vulkan.h"
#include "detail/vk_mem_alloc.h"
#include "../common.h"

#include <vector>
#include <string>
#include <thread>

BEGIN_GAIA_GRAPHICS_VK1

class Buffer;
class Texture;
class ShaderResourceView;
class UnorderedAccessView;
class RenderTargetView;
class DepthStencilView;
class Sampler;

//class VertexShader;
//class FragmentShader;
//class GeometryShader;
//class HullShader;
//class DomainShader;
//class ComputeShader;
//class MeshShader;
//class AmplificationShader;
//class RayGenerationShader;
//class IntersectionShader;
//class AnyHitShader;
//class ClosestHitShader;
//class MissShader;
//class CallableShader;

class GraphicsState;
class ComputeState;
class RayTracingState;
class MeshShaderState;

class GraphicsPipelineState;
class ComputePipelineState;
class RayTracingPipelineState;
class MeshShaderPipelineState;

class RenderWindow;


typedef uint32_t(*SelectPhysicalDeviceFunc)(const std::vector<std::string>& deviceNames);

//struct VulkanConfig
//{
//	std::vector<std::string> instanceExtensions;
//	std::vector<std::string> deviceExtensions;
//};

struct RenderSystemConfig
{
	//VulkanConfig vulkanConfig;
	std::string appName;
	SelectPhysicalDeviceFunc selectPhysicalDeviceFunc{ nullptr };
	uint32_t recordThreadCount{ 0 };
	bool debug{ false };
	bool vsync{ false };
};

class RenderSystem
{
public:
	static RenderSystem* New();
public:
	RenderSystem() = default;
	~RenderSystem();
	RenderSystem(const RenderSystem&) = delete;
	RenderSystem& operator=(const RenderSystem&) = delete;
	RenderSystem(RenderSystem&&) = default;
	RenderSystem& operator=(RenderSystem&&) = default;
public:
	bool initialize(const RenderSystemConfig& config);
	Buffer* createBuffer(const BufferDesc& desc);
	void writeBuffer(Buffer* buffer, uint64_t offset, uint64_t size, const void* data);

	//VertexShader* createVertexShader(const char* code, size_t codeSize, const char* entryPoint);
	//FragmentShader* createFragmentShader(const char* code, size_t codeSize, const char* entryPoint);
	//GeometryShader* createGeometryShader(const char* code, size_t codeSize, const char* entryPoint);
	//HullShader* createHullShader(const char* code, size_t codeSize, const char* entryPoint);
	//DomainShader* createDomainShader(const char* code, size_t codeSize, const char* entryPoint);
	//ComputeShader* createComputeShader(const char* code, size_t codeSize, const char* entryPoint);
	//MeshShader* createMeshShader(const char* code, size_t codeSize, const char* entryPoint);
	//AmplificationShader* createAmplificationShader(const char* code, size_t codeSize, const char* entryPoint);
	//RayGenerationShader* createRayGenerationShader(const char* code, size_t codeSize, const char* entryPoint);
	//IntersectionShader* createIntersectionShader(const char* code, size_t codeSize, const char* entryPoint);
	//AnyHitShader* createAnyHitShader(const char* code, size_t codeSize, const char* entryPoint);
	//ClosestHitShader* createClosestHitShader(const char* code, size_t codeSize, const char* entryPoint);
	//MissShader* createMissShader(const char* code, size_t codeSize, const char* entryPoint);
	//CallableShader* createCallableShader(const char* code, size_t codeSize, const char* entryPoint);

	GraphicsPipelineState* createGraphicsPipelineState(const GraphicsPipelineDesc& desc);
	ComputePipelineState* createComputePipelineState(const ComputePipelineDesc& desc);
	RayTracingPipelineState* createRayTracingPipelineState(const RayTracingPipelineDesc& desc);
	MeshShaderPipelineState* createMeshShaderPipelineState(const MeshShaderPipelineDesc& desc);


	//GraphicsPipelineState* createGraphicsPipelineState(const GraphicsPipelineStateDesc& desc);
	//ComputePipelineState* createComputePipelineState(const ComputePipelineStateDesc& desc);
	//RayTracingPipelineState* createRayTracingPipelineState(const RayTracingPipelineStateDesc& desc);
	//MeshShaderPipelineState* createMeshShaderPipelineState(const MeshShaderPipelineStateDesc& desc);

	RenderWindow* createRenderWindow(void* platformHandle);
public:
	void render(VertexShaderElement* graphicsElements);
private:
	struct RecordThread
	{
		std::thread thread;
		VkCommandPool commandPool;
		VkCommandBuffer commandBuffer;
	};
	struct TransferThread
	{
		std::thread thread;
		VkCommandPool commandPool;
		VkCommandBuffer commandBuffer;
	};
private:
	VkInstance m_instance{ VK_NULL_HANDLE };
	VkPhysicalDevice m_physicalDevice{ VK_NULL_HANDLE };
	VkDevice m_device{ VK_NULL_HANDLE };
	VkPhysicalDeviceFeatures enabledFeatures{};
	VkQueue m_graphicsQueue{ VK_NULL_HANDLE };
	VkQueue m_computeQueue{ VK_NULL_HANDLE };
	VkQueue m_transferQueue{ VK_NULL_HANDLE };
	VmaAllocator m_allocator{ VK_NULL_HANDLE };
	uint32_t m_graphicsQueueFamilyIndex{ 0 };
	uint32_t m_computeQueueFamilyIndex{ 0 };
	uint32_t m_transferQueueFamilyIndex{ 0 };
	std::vector<RecordThread> m_recordThreads;
	TransferThread m_transferThread;
};

END_GAIA_GRAPHICS_VK1
