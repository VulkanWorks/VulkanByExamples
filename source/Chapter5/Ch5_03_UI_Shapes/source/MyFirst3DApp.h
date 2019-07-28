#pragma once
#include "../../../common/VulkanApp.h"
#include "../../../common/SceneGraph/Scene3D.h"

class RectangleFactory;
class MyFirst3DApp : public VulkanApp
{
public:
    MyFirst3DApp();
    virtual ~MyFirst3DApp();

    // Core virtual methods used by derived classes
    virtual void Configure();
    virtual void Setup();
    virtual void Update();

	virtual void mousePressEvent(QMouseEvent* p_Event);
	virtual void mouseReleaseEvent(QMouseEvent* p_Event);
	virtual void mouseMoveEvent(QMouseEvent* p_Event);

private:
	void Grid(Scene3D* m_Scene);
	void MixerView(Scene3D* m_Scene);
	void ProgressBarFunc(Scene3D* m_Scene);

private:
    //RectangleFactory* m_CubeFactory;
	///////////////////
	Scene3D* m_Scene;
	Model3D* m_Cube1;
	Model3D* m_Cube2;
};

