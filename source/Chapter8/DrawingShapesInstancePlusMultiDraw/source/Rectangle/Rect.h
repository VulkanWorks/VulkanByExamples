#pragma once
#include "../../../../common/VulkanApp.h"

#include "../../DrawingShapes_SceneGraph/Node.h"

class Rectangl : public Node
{
public:
    enum DRAW_TYPE
    {
        FILLED = 0,
        OUTLINE,
        ROUNDED,
        DRAW_TYPE_COUNT
    };

public:
    Rectangl(Scene* p_Scene, Node* p_Parent, const BoundingRegion& p_BoundedRegion, const QString& p_Name = "", SHAPE p_ShapeType = SHAPE::SHAPE_NONE);
    virtual ~Rectangl() {}

    GETSET(DRAW_TYPE, DrawType)
        
protected:
    virtual void Update(Node* p_Item = NULL)
    {
        Node::Update(p_Item);

        if (m_ShapeType == SHAPE::SHAPE_RECTANGLE_MULTIDRAW)
        CreateRectVertexBuffer();
    }

public:
    virtual RenderSchemeFactory* GetRenderSchemeFactory();

    virtual void Setup();
    void CreateRectVertexBuffer();
    VulkanBuffer m_VertexBuffer;
};
