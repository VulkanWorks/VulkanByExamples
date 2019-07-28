#pragma once

#include <QObject>
#include <QMap>
#include "Transformation.h"
#include "../../../common/VulkanHelper.h"
#include "Common.h"

class Node;
class Scene;
class QMouseEvent;
class RenderSchemeFactory;
class AbstractApp;

class Scene
{
public:
    Scene(AbstractApp* p_Application = NULL, const QString& p_Name = QString());
    virtual ~Scene();

    void Setup();
    void SetupRenderFactory(VkCommandBuffer& p_CommandBuffer);
    void Update();
    void Render(VkCommandBuffer& p_CommandBuffer);

    void AddItem(Node* p_Item);
    void RemoveItem(Node* p_Item);

    virtual void Resize(VkCommandBuffer& p_CommandBuffer, int p_Width, int p_Height);
    virtual void SetUpProjection();
    inline Transformation& Transform() { return m_Transform; }

    void PushMatrix() { m_Transform.PushMatrix(); }
    void PopMatrix() { m_Transform.PopMatrix(); }
    void ApplyTransformation(const glm::mat4& m_TransformationMatrix) { *m_Transform.GetModelMatrix() *= m_TransformationMatrix; }

    virtual void mousePressEvent(QMouseEvent* p_Event);
    virtual void mouseReleaseEvent(QMouseEvent* p_Event);
    virtual void mouseMoveEvent(QMouseEvent* p_Event);

    RenderSchemeFactory* GetRenderSchemeFactory(Node* p_Item);
    void AppendToFlatNodeList(Node* p_Item);

private:
    void GatherFlatNodeList();

    std::vector<Node*>                      m_NodeList;
    std::vector<Node*>                      m_FlatList;
    std::set<RenderSchemeFactory*>          m_RenderSchemeFactorySet;
    std::map<SHAPE, RenderSchemeFactory*>   m_ShapeRenderSchemeTypeMap;

    GETSET(QString,                         Name)
    GETSET(Node*,                           CurrentHoverItem)  // Not owned by Scene
    GETSET(AbstractApp*,                    Application)
    GETSET(int,                             ScreenHeight);
    GETSET(int,                             ScreenWidth);
    GETSET(int,                             Frame);
    GETSET(Transformation,                  Transform);
};
