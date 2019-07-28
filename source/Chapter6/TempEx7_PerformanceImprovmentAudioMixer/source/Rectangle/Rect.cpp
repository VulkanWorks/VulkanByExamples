#include "Rect.h"

#include "RectangleGeometry.h"
#include "RectangleInstancingScheme.h"
#include "RectangleShaderTypes.h"

Rectangl::Rectangl(Scene *p_Scene, Node *p_Parent, const BoundingRegion& p_BoundedRegion, const QString &p_Name, SHAPE p_ShapeType)
    : Node(p_Scene, p_Parent, p_BoundedRegion, p_Name, p_ShapeType)
    , m_DrawType(FILLED)
{
}

RenderSchemeFactory* Rectangl::GetRenderSchemeFactory()
{
    return new RectangleInstancingScheme(static_cast<VulkanApp*>(m_Scene->GetApplication()));
}
