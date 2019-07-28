#pragma once
#include "../../../common/VulkanApp.h"
#include "../TempEx1_SceneGraph/Scene3D.h"

class RectangleModel;
class InstancingDemoApp : public VulkanApp
{
public:
    InstancingDemoApp();
    virtual ~InstancingDemoApp();

    // Core virtual methods used by derived classes
    virtual void Configure();
    virtual void Setup();
    virtual void Update();
    virtual bool Render();
    void ResizeWindow(int width, int height);

private:
	Scene3D* m_Scene;
    RectangleModel* m_Cube;
};
    