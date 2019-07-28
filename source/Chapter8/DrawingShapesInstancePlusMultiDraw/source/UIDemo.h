#pragma once
#include "../DrawingShapes_SceneGraph/Scene.h"
#include "../DrawingShapes_SceneGraph/Node.h"

#define CIRCLE_DEFINED 0

#if CIRCLE_DEFINED == 1
class Circle;
#endif

class Rectangl;
class TransformationConformTest;
class QMouseEvent;

class ProgressBar : public Node
{
public:
    ProgressBar(Scene* p_Scene, Node* p_Parent, const BoundingRegion& p_BoundedRegion, const QString& p_Name = "", SHAPE p_ShapeType = SHAPE::SHAPE_NONE);
    virtual ~ProgressBar() {}

signals:
    void ValueChanged(int p_NewValue);

protected:
    virtual void mousePressEvent(QMouseEvent* p_Event);
    virtual void mouseMoveEvent(QMouseEvent* p_Event);
    virtual void mouseReleaseEvent(QMouseEvent* p_Event);
    virtual void EmitValueChanged(int p_NewValue);

public:
    void SetRange(int p_Min, int p_Max);
    void SetValue(int p_Value);

    inline int GetValue() const { return m_Value; }
    inline int GetMinimum() const { return m_Min; }
    inline int GetMaximum() const { return m_Max; }

    Rectangl* progressIndicator;
    Rectangl* bar;

    int m_Min;
    int m_Max;
    int m_Value;
};

class AudioMixerItem : public Node
{
public:
    AudioMixerItem(Scene* p_Scene, Node* p_Parent, const BoundingRegion& p_BoundedRegion, const QString& p_Name, SHAPE p_ShapeType = SHAPE::SHAPE_NONE);
    virtual ~AudioMixerItem() {}

    //virtual void mouseMoveEvent(QMouseEvent* p_Event);
    //Model3D* progressIndicator;
};

class TransformationConformTest
{
public:
    TransformationConformTest();
    ~TransformationConformTest(){}

    void Configure(Scene* p_Scene);
    void Update();

private:
    Rectangl* m_RectTr1;
    Rectangl* m_RectTr2;
    Rectangl* m_RectTr3;
    Rectangl* m_RectTr4;

#if CIRCLE_DEFINED == 1
    Circle*   m_CircleTr5;
#endif

    Rectangl* m_Rect1;
    Rectangl* m_Rect2;
    Rectangl* m_Rect3;
    Rectangl* m_Rect4;
};

class UIDemo
{
public:
    UIDemo();
    virtual ~UIDemo();

    void Grid(Scene* p_Scene, int p_Width, int p_Height);
    void MixerView(Scene* p_Scene, int p_Width, int p_Height);
    void ProgressBarFunc(Scene* p_Scene);

    void InitTransformationConformTest(Scene* p_Scene) { m_TransformObj.Configure(p_Scene); }
    void UpdateTransformationConformTest() { m_TransformObj.Update(); }

private:
    TransformationConformTest m_TransformObj;
};
