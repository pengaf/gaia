#include "single_thread_render_scheduler.h"
#include "../vertex_shader_element.h"

BEGIN_GAIA_VK1

SingleThreadRenderScheduler::SingleThreadRenderScheduler()
{}

SingleThreadRenderScheduler::~SingleThreadRenderScheduler()
{
}

void SingleThreadRenderScheduler::render(RenderWindow** renderWindows, uint32_t count)
{
	for (uint32_t i = 0; i < count; ++i)
	{
		RenderWindow* renderWindow = renderWindows[i];
	}

	VkCommandBuffer commandBuffer;
	VkPipeline currentPipeline = VK_NULL_HANDLE;
	VertexBufferSet currentVertexBufferSet;
	IndexBuffer currentIndexBuffer;

	VkRenderingInfo renderingInfo;
	vkCmdBeginRendering(commandBuffer, &renderingInfo);

	VertexShaderElement* graphicsElement;
	VkPipeline pipeline = graphicsElement->m_pipeline;
	if (currentPipeline != pipeline)
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		currentPipeline = pipeline;
	}

	const VertexBufferSet& vertexBufferSet = graphicsElement->m_vertexBufferSet;
	if (currentVertexBufferSet != vertexBufferSet)
	{
		vkCmdBindVertexBuffers(commandBuffer, vertexBufferSet.firstBinding, vertexBufferSet.bindingCount, vertexBufferSet.buffers, vertexBufferSet.offsets);
		currentVertexBufferSet = vertexBufferSet;
	}

	const IndexBuffer& indexBuffer = graphicsElement->m_indexBuffer;
	if (currentIndexBuffer != indexBuffer)
	{
		vkCmdBindIndexBuffer2(commandBuffer, indexBuffer.buffer, indexBuffer.offset, indexBuffer.size, indexBuffer.type);
		currentIndexBuffer = indexBuffer;
	}


	vkCmdEndRendering(commandBuffer);

}

END_GAIA_VK1