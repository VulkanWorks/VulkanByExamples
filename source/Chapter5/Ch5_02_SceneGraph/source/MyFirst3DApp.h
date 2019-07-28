#pragma once
#include "../../../common/VulkanApp.h"
#include "../../../common/SceneGraph/Scene3D.h"

class CubeFactory;
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
    CubeFactory* m_CubeFactory;
	///////////////////
	Scene3D* m_Scene;
	Model3D* m_Cube1;
	Model3D* m_Cube2;
};

