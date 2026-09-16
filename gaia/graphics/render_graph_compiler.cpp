#include "render_graph_compiler.h"
#include "render_pass.h"

BEGIN_GAIA

RenderGraphCompiler::ErrorInfo RenderGraphCompiler::compile(RenderGraph* renderGraph)
{
	ErrorInfo errorInfo;
	m_resoureWritePass.clear();
	m_resoureReadPasses.clear();

	for (RenderPass* renderPass : renderGraph->m_renderPasses)
	{
		switch (renderPass->kind())
		{
		case RenderPassKind::scene_pass: {
			ScenePass* scenePass = static_cast<ScenePass*>(renderPass);
			if (!m_resoureWritePass.insert(std::make_pair(scenePass->m_dsv, scenePass)).second)
			{
				errorInfo.errorCode = RenderGraphErrorCode::resource_multi_written;
				errorInfo.resource = scenePass->m_dsv;
				errorInfo.renderPass = renderPass;
				return errorInfo;
			}
			for (uint32_t rtv : scenePass->m_rtvs)
			{
				if (!m_resoureWritePass.insert(std::make_pair(rtv, scenePass)).second)
				{
					errorInfo.errorCode = RenderGraphErrorCode::resource_multi_written;
					errorInfo.resource = rtv;
					errorInfo.renderPass = renderPass;
					return errorInfo;
				}
			}
			break;}	
		case RenderPassKind::image_pass:
			break;
		case RenderPassKind::compute_pass:
			break;
		case RenderPassKind::ray_tracing_pass:
			break;
		}
	}
}

END_GAIA