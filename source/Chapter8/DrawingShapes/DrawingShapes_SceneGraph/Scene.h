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
    void Update();
    void Render(VkCommandBuffer& p_CommandBuffer); // TODO do not expose implementation here, Vulkan must be abstracted

    void AddItem(Node* p_Item);
    void RemoveItem(Node* p_Item);

    virtual void Resize(VkCommandBuffer& p_CommandBuffer, int p_Width, int p_Height);
    virtual void SetUpProjection();
    inline Transformation& Transform() { return m_Transform; }

    bool IsDirty() { return (m_DirtyType != SCENE_DIRTY_TYPE::NONE); }
    SCENE_DIRTY_TYPE GetDirtyType() { return m_DirtyType; }
    void SetDirtyType(SCENE_DIRTY_TYPE p_InvalidateType) { m_DirtyType = p_InvalidateType; }

    virtual void mousePressEvent(QMouseEvent* p_Event);
    virtual void mouseReleaseEvent(QMouseEvent* p_Event);
    virtual void mouseMoveEvent(QMouseEvent* p_Event);

    void PushMatrix() { m_Transform.PushMatrix(); }
    void PopMatrix() { m_Transform.PopMatrix(); }
    void ApplyTransformation(const glm::mat4& p_TransformationMatrix) { *m_Transform.GetModelMatrix() *= p_TransformationMatrix; }

private:
    RenderSchemeFactory* GetRenderSchemeFactory(Node* p_Item);
    void AppendToFlatNodeList(Node* p_Item);
    void GatherFlatNodeList();

private:
    std::vector<Node*>                      m_RootDrawableList;
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
    GETSET(bool,                            EarlyDepthTest);

private:
    SCENE_DIRTY_TYPE m_DirtyType;
    friend class Node;
};
