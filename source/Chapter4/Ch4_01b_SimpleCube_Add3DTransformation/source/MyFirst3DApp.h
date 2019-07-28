#pragma once
#include "../../../common/VulkanApp.h"

class Cube;
class MyFirst3DApp : public VulkanApp
{
public:
    MyFirst3DApp();
    virtual ~MyFirst3DApp();

    // Core virtual methods used by derived classes
    virtual void Configure();
    virtual void Setup();
    virtual void Update();

private:
    Cube* m_Cube;
};

