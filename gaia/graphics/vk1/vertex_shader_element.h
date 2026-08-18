#pragma once
#include "render_element.h"
#include "../common.h"

BEGIN_GAIA_GRAPHICS_VK1

class GraphicsPipeline;
class Material;
class Mesh;

struct VertexBufferSet
{
	uint32_t firstBinding{ 0 };
	uint32_t bindingCount{ 0 };
	VkBuffer buffers[gaia_max_vertex_buffer_bind_count];
	VkDeviceSize offsets[gaia_max_vertex_buffer_bind_count];
	bool operator == (const VertexBufferSet& other) const
	{
		if (firstBinding != other.firstBinding ||
			bindingCount != other.bindingCount)
		{
			return false;
		}
		for (uint32_t i = 0; i < bindingCount; ++i)
		{
			if (buffers[i] != other.buffers[i] ||
				offsets[i] != othre.offsets[i])
			{
				return false;
			}
		}
		return true;
	}

	bool operator != (const VertexBufferSet& other) const
	{
		return !(*this == other);
	}

	VertexBufferSet& operator = (const VertexBufferSet& other)
	{
		firstBinding = other.firstBinding;
		const uint32_t srcCount = other.bindingCount;
		bindingCount = srcCount;
		for (uint32_t i = 0; i < srcCount; ++i)
		{
			buffers[i] = other.buffers[i];
			offsets[i] = other.offsets[i];
		}
		return *this;
	}
};

struct IndexBuffer
{
	VkBuffer buffer{ VK_NULL_HANDLE };
	VkDeviceSize offset{ 0 };
	VkDeviceSize size{ 0 };
	VkIndexType type{ VK_INDEX_TYPE_MAX_ENUM };
	bool operator == (const IndexBuffer& other) const
	{
		return buffer == other.buffer
			&& offset == other.offset
			&& size == other.size
			&& type == other.type;
	}
};

class VertexShaderElement : public RenderElement
{
public:
	VertexShaderElement() : RenderElement(RenderElementType::vertex_shader_element) 
	{}
	~VertexShaderElement() = default;
	VertexShaderElement(const VertexShaderElement&) = delete;
	VertexShaderElement& operator=(const VertexShaderElement&) = delete;
	VertexShaderElement(VertexShaderElement&&) = default;
	VertexShaderElement& operator=(VertexShaderElement&&) = default;
public:
	void setGraphicsPipeline(GraphicsPipeline* graphicsPipeline);
	void setMaterial(Material* material);
	void setMesh(Mesh* mesh);
	void setDrawCall(const DrawCall& drawCall);
	void setStencilRef(uint32_t stencilRef);
	void setBlendFactor(const float blendFactor[4]);
	void setIndirectBuffer(Buffer* buffer, uint64_t offset);
	//void setCountBuffer(Buffer* buffer, uint64_t offset);
public:
	void setVertexBuffers(const Buffer* const*  buffers, const uint64_t* offsets, uint32_t count);
	void setIndexBuffer(Buffer* buffer, uint64_t offset, IndexType indexType);
	void setConstant(const char* name, const void* data, uint32_t size);
	void setResource(const char* name, ShaderResourceView* shaderResourceView);
public:
	VkPipeline m_pipeline{ VK_NULL_HANDLE };
	VertexBufferSet m_vertexBufferSet;
	IndexBuffer m_indexBuffer;
	//VkBuffer m_vertexBuffers[gaia_max_vertex_buffer_bind_count]{ VK_NULL_HANDLE };
	//VkDeviceSize m_vertexBufferOffsets[gaia_max_vertex_buffer_bind_count]{ 0 };
	//VkBuffer m_indexBuffer{ VK_NULL_HANDLE };
	//VkDeviceSize m_indexBufferOffset{ 0 };
	//VkIndexType m_indexType{ VK_INDEX_TYPE_UINT16 };

	VkBuffer m_indirectBuffer{ VK_NULL_HANDLE };
	//VkBuffer m_countBuffer{ VK_NULL_HANDLE };
public:
	static VertexShaderElement* New(RenderSystem* renderSystem, GraphicsPipelineState* pipelineState);
};


END_GAIA_GRAPHICS_VK1