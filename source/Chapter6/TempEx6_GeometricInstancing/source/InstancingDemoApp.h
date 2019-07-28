#pragma once
#include "../../../common/VulkanApp.h"

class Rect;
class InstancingDemoApp : public VulkanApp
{
public:
    InstancingDemoApp();
    virtual ~InstancingDemoApp();

    // Core virtual methods used by derived classes
    virtual void Configure();
    virtual void Setup();
    virtual void Update();
    void ResizeWindow(int width, int height);

private:
    Rect* m_Cube;
};

