#pragma once
#include "../../../common/VulkanApp.h"
class SimpleMesh;
class MeshDemoApp : public VulkanApp
{
public:
	MeshDemoApp();
	virtual ~MeshDemoApp();

	// Core virtual methods used by derived classes
	void Configure();
	void Setup();
	void Update();
    void ResizeWindow(int width, int height);

private:
	SimpleMesh* m_Cube;
};
