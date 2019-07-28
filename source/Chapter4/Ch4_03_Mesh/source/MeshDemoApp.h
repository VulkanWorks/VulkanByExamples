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

private:
	SimpleMesh* m_Cube;
};
