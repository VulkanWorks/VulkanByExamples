#pragma once
#include "../TempEx2_SceneGraph/Scene3D.h"
#include "Rect.h"

class UIDemo
{
public:
    UIDemo();
    virtual ~UIDemo();

	void Grid(Scene3D* p_Scene, int p_Width, int p_Height);
	void MixerView(Scene3D* m_Scene, int p_Width, int p_Height);
//	void ProgressBarFunc(Scene3D* m_Scene);
};
    
class RectangleModel;
//class QMouseEvent;
//class ProgressBar : public Model3D
//{
//public:
//    ProgressBar(Scene3D* p_Scene, Model3D* p_Parent, const QString& p_Name = "", SHAPE p_ShapeType = SHAPE::SHAPE_NONE);
//    virtual ~ProgressBar() {}
//
//    virtual bool mouseMoveEvent(QMouseEvent* p_Event);
//    RectangleModel* progressIndicator;
//    RectangleModel* bar;
//};

class AudioMixerItem : public Model3D
{
public:
    AudioMixerItem(Scene3D* p_Scene, Model3D* p_Parent, const BoundingRegion& p_BoundedRegion, const QString& p_Name, SHAPE p_ShapeType = SHAPE::SHAPE_NONE);
    virtual ~AudioMixerItem() {}

    //virtual bool mouseMoveEvent(QMouseEvent* p_Event);
    //Model3D* progressIndicator;
};

