#include "Rect.h"

#include "RectangleDescriptorSet.h"
#include "RectangleGeometry.h"
#include "RectangleMultiDrawScheme.h"
#include "RectangleInstancingScheme.h"
#include "RectangleShaderTypes.h"

Rectangl::Rectangl(Scene *p_Scene, Node *p_Parent, const BoundingRegion& p_BoundedRegion, const QString& p_Name, SHAPE p_ShapeType)
    : Node(p_Scene, p_Parent, p_BoundedRegion, p_Name, p_ShapeType)
    , m_DrawType(FILLED)
{
}

RenderSchemeFactory* Rectangl::GetRenderSchemeFactory()
{
    if (m_ShapeType == SHAPE::SHAPE_RECTANGLE_MULTIDRAW)
    {
        return new RectangleMultiDrawScheme(static_cast<VulkanApp*>(m_Scene->GetApplication()));
    }
    else if (m_ShapeType == SHAPE::SHAPE_RECTANGLE_INSTANCED)
    {
        return new RectangleInstancingScheme(static_cast<VulkanApp*>(m_Scene->GetApplication()));
    }

    assert(false);
    return NULL;
}

void Rectangl::Setup()
{
    // Need proper cast for shape type class enum
    if (m_ShapeType == SHAPE::SHAPE_RECTANGLE_MULTIDRAW)
    {
        CreateRectVertexBuffer();
    }

    Node::Setup();
}

void Rectangl::CreateRectVertexBuffer()
{
    glm::mat4 parentTransform = GetAbsoluteTransformation();//m_Model * GetParentsTransformation(GetParent());

    Vertex rectVertices[6];
    memcpy(rectVertices, rectFilledVertices, sizeof(Vertex) * 6);
    uint32_t dataSize = sizeof(rectVertices);
    uint32_t dataStride = sizeof(rectVertices[0]);
    const int vertexCount = dataSize / dataStride;
    for (int i = 0; i < vertexCount; ++i)
    {
        glm::vec4 pos(rectFilledVertices[i].m_Position, 1.0);
        pos.x = pos.x * m_BoundedRegion.m_Dimension.x;
        pos.y = pos.y * m_BoundedRegion.m_Dimension.y;

        pos = parentTransform * pos;
        //std::cout << m_Name.toStdString() << "=+ x:" << pos.x << ", y:" << pos.y << ", z:" << pos.z << endl;

        rectVertices[i].m_Position.x = pos.x;
        rectVertices[i].m_Position.y = pos.y;
        rectVertices[i].m_Position.z = pos.z;
    }

    VulkanApp* app = static_cast<VulkanApp*>(m_Scene->GetApplication());
    const VkDevice& device = app->m_hDevice;

    if (m_VertexBuffer.m_Buffer != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(device, m_VertexBuffer.m_Buffer, NULL);
        vkFreeMemory(device, m_VertexBuffer.m_Memory, NULL);
    }

    m_VertexBuffer.m_DataSize = dataSize;
    m_VertexBuffer.m_MemoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

    const VkPhysicalDeviceMemoryProperties& memProp = app->m_physicalDeviceInfo.memProp;
    VulkanHelper::CreateBuffer(device, memProp, m_VertexBuffer, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, rectVertices);
}
