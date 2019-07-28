#include "Node.h"
#include<QMouseEvent>
#include<glm/gtx/string_cast.hpp>

Node::Node(Scene* p_Scene, Node* p_Parent, const BoundingRegion& p_BoundedRegion, const QString& p_Name, SHAPE p_ShapeType)
    : m_Scene(p_Scene)
    , m_Parent(p_Parent)
    , m_Name(p_Name)
    , m_ShapeType(p_ShapeType)
    , m_BoundedRegion(p_BoundedRegion)
    , m_OriginOffset(glm::vec3(0.0f, 0.0f, 0.0f))
{
    m_Parent ? m_Parent->m_ChildList.append(this) : p_Scene->AddItem(this);
    
    // Todo: We can directly use the translate as the m_BoundedRegion is already set
    SetGeometry(m_BoundedRegion.m_Position.x, m_BoundedRegion.m_Position.y, m_BoundedRegion.m_Dimension.x, m_BoundedRegion.m_Dimension.y, m_BoundedRegion.m_Position.z);
}

void Node::Setup()
{
    foreach(Node* childItem, m_ChildList)
    {
        if (!childItem) continue;

        childItem->Setup();
    }
}

// p_Item != NULL => Update is performed w.r.t. root parent
// p_Item == NULL => Update is performed w.r.t. p_Item's parent
void Node::Update(Node* p_Item)
{
    m_Scene->PushMatrix();
    if (p_Item)
    {
        m_Scene->ApplyTransformation(p_Item->GetParentsTransformation(GetParent()) * m_ModelTransformation); // This retrives all the transformation from the parent
    }
    else
    {
        m_Scene->ApplyTransformation(m_ModelTransformation);
    }

    m_AbsoluteTransformation = *m_Scene->GetRefTransform().GetModelMatrix();

    QList<Node*>& childList = (p_Item ? p_Item->m_ChildList : m_ChildList);
    Q_FOREACH(Node* childItem, childList)
    {
        if (!childItem) continue;

        childItem->Update();
    }

    m_Scene->PopMatrix();
}

void Node::Rotate(float p_Angle, float p_X, float p_Y, float p_Z)
{
    if (m_OriginOffset != glm::vec3(0.0f, 0.0f, 0.0f))
    {
        m_ModelTransformation = glm::translate(m_ModelTransformation, m_OriginOffset);
    }

    m_ModelTransformation = glm::rotate(m_ModelTransformation, p_Angle, glm::vec3(p_X, p_Y, p_Z));

    if (m_OriginOffset != glm::vec3(0.0f, 0.0f, 0.0f))
    {
        m_ModelTransformation = glm::translate(m_ModelTransformation, -m_OriginOffset);
    }
}

void Node::Translate(float p_X, float p_Y, float p_Z)
{
    if (m_OriginOffset != glm::vec3(0.0f, 0.0f, 0.0f))
    {
        m_ModelTransformation = glm::translate(m_ModelTransformation, m_OriginOffset);
    }

    m_ModelTransformation = glm::translate(m_ModelTransformation, glm::vec3(p_X, p_Y, p_Z));

    if (m_OriginOffset != glm::vec3(0.0f, 0.0f, 0.0f))
    {
        m_ModelTransformation = glm::translate(m_ModelTransformation, -m_OriginOffset);
    }
}

void Node::Scale(float p_X, float p_Y, float p_Z)
{
    if (m_OriginOffset != glm::vec3(0.0f, 0.0f, 0.0f))
    {
        m_ModelTransformation = glm::translate(m_ModelTransformation, m_OriginOffset);
    }

    m_ModelTransformation = glm::scale(m_ModelTransformation, glm::vec3(p_X, p_Y, p_Z));

    if (m_OriginOffset != glm::vec3(0.0f, 0.0f, 0.0f))
    {
        m_ModelTransformation = glm::translate(m_ModelTransformation, -m_OriginOffset);
    }
}

void Node::ResetPosition()
{
    Reset();
    Translate(m_BoundedRegion.m_Position.x, m_BoundedRegion.m_Position.y, m_BoundedRegion.m_Position.z);
    m_AbsoluteTransformation = m_ModelTransformation * GetParentsTransformation(GetParent());
}

void Node::SetZOrder(float p_ZOrder)
{
    m_BoundedRegion.m_Position.z = p_ZOrder;

    Reset();
    Translate(m_BoundedRegion.m_Position.x, m_BoundedRegion.m_Position.y, m_BoundedRegion.m_Position.z);
}

void Node::SetPosition(float p_X, float p_Y)
{
    glm::vec4 posStart((0 * m_BoundedRegion.m_Dimension.x), (0 * m_BoundedRegion.m_Dimension.y), 0.0, 1.0);
    glm::vec4 posStartResult = m_AbsoluteTransformation * posStart;

    glm::vec4 posEnd((m_BoundedRegion.m_Dimension.x), (m_BoundedRegion.m_Dimension.y), 0.0, 1.0);
    glm::vec4 posEndResult = m_AbsoluteTransformation * posEnd;

    m_BoundedRegion.m_Position.x = p_X;
    m_BoundedRegion.m_Position.y = p_Y;

    ResetPosition();
}

void Node::SetGeometry(float p_X, float p_Y, float p_Width, float p_Height, float p_ZOrder/*=0*/)
{
    Translate(p_X, p_Y, p_ZOrder);

    m_BoundedRegion.m_Position.x = p_X;
    m_BoundedRegion.m_Position.y = p_Y;
    m_BoundedRegion.m_Position.z = p_ZOrder;

    m_BoundedRegion.m_Dimension.x = p_Width;
    m_BoundedRegion.m_Dimension.y = p_Height;
}

void Node::mousePressEvent(QMouseEvent* p_Event)
{
    glm::vec4 posStart((0 * m_BoundedRegion.m_Dimension.x), (0 * m_BoundedRegion.m_Dimension.y), 0.0, 1.0);
    glm::vec4 posStartResult = m_AbsoluteTransformation * posStart;

    glm::vec4 posEnd((m_BoundedRegion.m_Dimension.x), (m_BoundedRegion.m_Dimension.y), 0.0, 1.0);
    glm::vec4 posEndResult = m_AbsoluteTransformation * posEnd;

    cout << "\n##### mousePressEventS" << glm::to_string(posStartResult);// << posEndResult;
    cout << "\n##### mousePressEventE" << glm::to_string(posEndResult);// << posEndResult;

    QRect rect(QPoint(posStartResult.x, posStartResult.y), QPoint(posEndResult.x, posEndResult.y));
    if (rect.contains(p_Event->x(), p_Event->y()))
        cout << "\n***************";

    foreach(Node* childItem, m_ChildList)
    {
        assert(childItem);

        childItem->mousePressEvent(p_Event);
    }
}

void Node::mouseReleaseEvent(QMouseEvent* p_Event)
{
    foreach(Node* childItem, m_ChildList)
    {
        assert(childItem);

        childItem->mouseReleaseEvent(p_Event);
    }
}

void Node::mouseMoveEvent(QMouseEvent* p_Event)
{
    glm::vec4 posStart((0 * m_BoundedRegion.m_Dimension.x), (0 * m_BoundedRegion.m_Dimension.y), 0.0, 1.0);
    glm::vec4 posStartResult = /*GetParentsTransformation(GetParent()) **/ m_AbsoluteTransformation * posStart;

    glm::vec4 posEnd((m_BoundedRegion.m_Dimension.x), (m_BoundedRegion.m_Dimension.y), 0.0, 1.0);
    glm::vec4 posEndResult = /*GetParentsTransformation(GetParent()) **/ m_AbsoluteTransformation * posEnd;

    QRect rect(QPoint(posStartResult.x, posStartResult.y), QPoint(posEndResult.x, posEndResult.y));
    if (rect.contains(p_Event->x(), p_Event->y()))
    {
        m_Scene->SetCurrentHoverItem(this);

        for (int i = m_ChildList.size() - 1; i >= 0; i--)
        {
            Node* childItem = m_ChildList.at(i);
            assert(childItem);

            childItem->mouseMoveEvent(p_Event);
        }

        p_Event->accept();
        return;
    }

    p_Event->ignore();
}

Node* Node::GetParent() const
{
    return m_Parent;
}

void Node::ApplyTransformation()
{
    *m_Scene->Transform().GetModelMatrix() *= m_ModelTransformation;
}

glm::mat4 Node::GetAbsoluteTransformations() const
{
    return GetParentsTransformation(GetParent()) * m_ModelTransformation;
}

glm::mat4 Node::GetParentsTransformation(Node *p_Parent) const
{
    return p_Parent ? (GetParentsTransformation(p_Parent->GetParent()) * p_Parent->m_ModelTransformation) : glm::mat4();
}

void Node::GatherFlatNodeList()
{
    if (!m_Scene) return;

    m_Scene->AppendToFlatNodeList(this);

    Q_FOREACH(Node* childItem, m_ChildList)
    {
        assert(childItem);
        childItem->GatherFlatNodeList();
    }
}
