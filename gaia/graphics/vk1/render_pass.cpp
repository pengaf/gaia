#include "render_pass.h"

BEGIN_GAIA_VK1

void BarrierCommand::execute(VkCommandBuffer commandBuffer)
{
    for (auto& pbc : m_pipelineBarrierCommands)
    {
        GAIA_ASSERT(0 == pbc.bufferMemoryBarrierCount || pbc.bufferMemoryBarrierCount + pbc.startBufferMemoryBarrier <= m_bufferBarriers.size());
        GAIA_ASSERT(0 == pbc.imageMemoryBarrierCount || pbc.imageMemoryBarrierCount + pbc.startImageMemoryBarrier <= m_imageBarriers.size());
        vkCmdPipelineBarrier(
            commandBuffer,
            pbc.srcStageMask,
            pbc.dstStageMask,
            pbc.dependencyFlags,
            0,
            nullptr,
            pbc.bufferMemoryBarrierCount,
            pbc.bufferMemoryBarrierCount ? m_bufferBarriers.data() + pbc.startBufferMemoryBarrier : nullptr,
            pbc.imageMemoryBarrierCount,
            pbc.imageMemoryBarrierCount ? m_imageBarriers.data() + pbc.startImageMemoryBarrier : nullptr);
    }
}

void SimplePipelineCommand::execute(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint)
{
    vkCmdBindPipeline(commandBuffer, bindPoint, m_pipeline);
    if (m_descriptorSetCount)
    {
        vkCmdBindDescriptorSets(
            commandBuffer,
            bindPoint,
            m_pipelineLayout,
            0,
            m_descriptorSetCount,
            m_descriptorSets,
            0,
            nullptr);
    }
}

void ImagePass::execute(VkCommandBuffer commandBuffer)
{
    BarrierCommand::execute(commandBuffer);
    vkCmdBeginRendering(commandBuffer, &m_renderingInfo);
    SimplePipelineCommand::execute(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS);
    vkCmdDraw(commandBuffer, m_drawCommand.vertexCount, m_drawCommand.instanceCount, m_drawCommand.firstVertex, m_drawCommand.firstInstance);
    vkCmdEndRendering(commandBuffer);
}

void ComputePass::execute(VkCommandBuffer commandBuffer)
{
    BarrierCommand::execute(commandBuffer);
    SimplePipelineCommand::execute(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE);
    vkCmdDispatch(commandBuffer, m_dispatchCommand.groupCountX, m_dispatchCommand.groupCountY, m_dispatchCommand.groupCountZ);
}

END_GAIA_VK1
