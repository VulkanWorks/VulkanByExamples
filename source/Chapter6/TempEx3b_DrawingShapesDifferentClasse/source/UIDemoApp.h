#pragma once
#include "../../../common/VulkanApp.h"
#include "../TempEx2_SceneGraph/Scene.h"
#include "UIDemo.h"

class Rectangl;
class Circle;
class UIDemoApp : public VulkanApp
{
public:
    UIDemoApp();
    virtual ~UIDemoApp();

    virtual void Configure();
    virtual void Setup();
    virtual void Update();
    virtual bool Render();
    virtual void ResizeWindow(int p_Width, int p_Height);

protected:
    virtual void mousePressEvent(QMouseEvent* p_Event);
    virtual void mouseReleaseEvent(QMouseEvent* p_Event);
    virtual void mouseMoveEvent(QMouseEvent* p_Event);

private:
    void RecordRenderPass(int p_Argcount, ...);

private:
    std::vector<std::shared_ptr<Scene>> m_SceneVector;
    Scene* m_Scene;
    Rectangl* m_RectTr1;
    Rectangl* m_RectTr2;
    Rectangl* m_RectTr3;
    Rectangl* m_RectTr4;
    Circle*   m_CircleTr5;

    Rectangl* m_Rect1;
    Rectangl* m_Rect2;
    Rectangl* m_Rect3;
    Rectangl* m_Rect4;
    UIDemo m_UIDemo;
};
