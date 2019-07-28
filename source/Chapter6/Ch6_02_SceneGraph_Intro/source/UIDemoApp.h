#pragma once
#include "../../../common/VulkanApp.h"
#include "../TempEx2_SceneGraphIntro/Scene.h"

class Rectangl;
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

private:
    void RecordRenderPass(int p_Argcount, ...);

private:
    std::vector<std::shared_ptr<Scene>> m_SceneVector;
    Scene* m_Scene;
    Rectangl* m_RectTr1;
    Rectangl* m_RectTr2;
    Rectangl* m_RectTr3;
    Rectangl* m_RectTr4;

    Rectangl* m_Rect1;
    Rectangl* m_Rect2;
    Rectangl* m_Rect3;
    Rectangl* m_Rect4;
};
