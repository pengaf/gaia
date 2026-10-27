#pragma once
#include "utility.h"
#include "detail/vulkan.h"
#include "detail/vk_mem_alloc.h"
#include "hal_fwd.h"

#include <vector>
#include <string>
#include <thread>

BEGIN_GAIA_VK1


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

class BufferHandle
{
public:
	BufferHandle(VkBuffer buffer, VmaAllocation allocation) :
		m_buffer(buffer),
		m_allocation(allocation)
	{
	}
	bool isValid() const
	{
		return m_buffer != VK_NULL_HANDLE;
	}
public:
	GAIA_ARG_RO(VkBuffer, buffer, VK_NULL_HANDLE);
	GAIA_ARG_RO(VmaAllocation, allocation, VK_NULL_HANDLE);
};

class TextureHandle
{
public:
	TextureHandle(VkImage image, VmaAllocation allocation) :
		m_image(image),
		m_allocation(allocation)
	{
	}
	bool isValid() const
	{
		return m_image != VK_NULL_HANDLE;
	}
public:
	GAIA_ARG_RO(VkImage, image, VK_NULL_HANDLE);
	GAIA_ARG_RO(VmaAllocation, allocation, VK_NULL_HANDLE);
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
	BufferHandle createBufferHandle(const BufferDesc& desc);
	void destroyBufferHandle(BufferHandle bufferHandle);
	TextureHandle createTextureHandle(const TextureDesc& desc);
	void destroyTextureHandle(TextureHandle textureHandle);

	//Buffer* createBuffer(const BufferDesc& desc);
	void writeBuffer(BufferHandle buffer, uint64_t offset, uint64_t size, const void* data);
	VertexShader* createVertexShader(const char* code, size_t codeSize, const char* entryPoint);

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

	GraphicsPipeline* createGraphicsPipelineState(const GraphicsPipelineDesc& desc);
	ComputePipeline* createComputePipelineState(const ComputePipelineDesc& desc);
	RayTracingPipeline* createRayTracingPipelineState(const RayTracingPipelineDesc& desc);
	MeshShaderPipeline* createMeshShaderPipelineState(const MeshShaderPipelineDesc& desc);


	//GraphicsPipeline* createGraphicsPipelineState(const GraphicsPipelineStateDesc& desc);
	//ComputePipeline* createComputePipelineState(const ComputePipelineStateDesc& desc);
	//RayTracingPipeline* createRayTracingPipelineState(const RayTracingPipelineStateDesc& desc);
	//MeshShaderPipeline* createMeshShaderPipelineState(const MeshShaderPipelineStateDesc& desc);

	RenderWindow* createRenderWindow(void* platformHandle);
public:
	VkDevice device() const
	{
		return m_device;
	}
	VmaAllocator allocator() const
	{
		return m_allocator;
	}
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

END_GAIA_VK1
