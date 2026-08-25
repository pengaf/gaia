#include "pass.h"
#include "hal/render_element.h"

BEGIN_GAIA

void RenderPassBase::addRenderElement(RenderElement* element)
{
	element->m_subCount = 0;
	m_elements.push_back(element);
}
	
void RenderPassBase::addRenderElementSequence(RenderElement** elements, size_t count)
{
	RenderElement* element = elements[0];
	element->m_subCount = static_cast<uint8_t>(count - 1);
	m_elements.push_back(element);
	if (count > 1)
	{
		element->m_subOffset = m_subElements.size();
		m_subElements.insert(m_subElements.end(), elements + 1, elements + count);
	}
}

void ComputePassBase::addComputeElement(ComputeElement* element)
{
	m_elements.push_back(element);
}

void RayTracingPassBase::addRayTracingElement(RayTracingElement* element)
{
	m_elements.push_back(element);
}

END_GAIA