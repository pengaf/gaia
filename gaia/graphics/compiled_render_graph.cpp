#include "compiled_render_graph.h"
#include <type_traits>
#include <utility>

BEGIN_GAIA

template<typename T>
T* CompiledRenderGraph::addPass_(std::string_view name)
{
	static_assert(std::is_base_of_v<Pass, T>, "T must derive from Pass");
	auto pass = std::make_unique<T>(name);
	T* res = pass.get();
	m_passes.push_back(std::move(pass));
	return res;
}

template<typename T, class...Args>
uint32_t CompiledRenderGraph::addResource_(Args&&... args)
{
	static_assert(std::is_base_of_v<Resource, T>, "T must derive from Resource");
	uint32_t handle = (uint32_t)m_resources.size();
	auto resource = std::make_unique<T>(std::forward<Args>(args)...);
	m_resources.push_back(std::move(resource));
	return handle;
}

uint32_t CompiledRenderGraph::addTexture(std::string_view name, const TextureDesc& desc)
{
	return addResource_<Texture>(name, desc);
}

uint32_t CompiledRenderGraph::addBuffer(std::string_view name, const BufferDesc& desc)
{
	return addResource_<Buffer>(name, desc);
}

uint32_t CompiledRenderGraph::cloneExternalTexture(ExternalTexture* externalTexture)
{
	return addResource_<ExternalTexture>(*externalTexture);
}

uint32_t CompiledRenderGraph::cloneExternalBuffer(ExternalBuffer* externalBuffer)
{
	return addResource_<ExternalBuffer>(*externalBuffer);
}

CompiledRenderGraph::ScenePass* CompiledRenderGraph::addScenePass(std::string_view name)
{
	return addPass_<ScenePass>(name);
}

CompiledRenderGraph::ImagePass* CompiledRenderGraph::addImagePass(std::string_view name)
{
	return addPass_<ImagePass>(name);
}

CompiledRenderGraph::ComputePass* CompiledRenderGraph::addComputePass(std::string_view name)
{
	return addPass_<ComputePass>(name);
}

CompiledRenderGraph::RayTracingPass* CompiledRenderGraph::addRayTracingPass(std::string_view name)
{
	return addPass_<RayTracingPass>(name);
}

CompiledRenderGraph::Pass* CompiledRenderGraph::addPassFromSource(const RenderGraphBase::Pass<false>* srcPass)
{
	Pass* dstPass = nullptr;
	switch (srcPass->kind())
	{
	case RenderPassKind::scene_pass: {
		ScenePass* scenePass = addScenePass(srcPass->name());
		dstPass = scenePass;
		break;}
	case RenderPassKind::image_pass:{
		ImagePass* imagePass = addImagePass(srcPass->name());
		imagePass->vertexCount(static_cast<const RenderGraphBase::ImagePass<false>*>(srcPass)->vertexCount());
		dstPass = imagePass;
		break;}
	case RenderPassKind::compute_pass: {
		auto* srcComputePass = static_cast<const RenderGraphBase::ComputePass<false>*>(srcPass);
		ComputePass* computePass = addComputePass(srcPass->name());
		computePass->setDispatch(srcComputePass->dispatchX(), srcComputePass->dispatchY(), srcComputePass->dispatchZ());
		dstPass = computePass;
		break;}
	case RenderPassKind::ray_tracing_pass:
		dstPass = addRayTracingPass(srcPass->name());
		break;
	default:
		GAIA_ASSERT(false && "Unknown pass kind");
		break;
	}
	return dstPass;
}

void CompiledRenderGraph::reset()
{
	m_resources.clear();
	m_passes.clear();
}

END_GAIA