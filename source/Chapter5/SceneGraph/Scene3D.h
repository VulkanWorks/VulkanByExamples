#pragma once

#include <QObject>
#include <QMatrix4x4>

#include "Transformation3D.h"
#include "../VulkanHelper.h"

class Model3D;
class Scene3D;
class QMouseEvent;

class Scene3D
{
public:
    Scene3D(const QString& p_Name = "");
    virtual ~Scene3D();

    void Setup();
    void Update();

    void AddModel(Model3D* p_Model);
    void RemoveModel(Model3D *p_Model);

    void Resize(int p_Width, int p_Height);
    void SetUpProjection();
    inline Transformation3D& Transform() { return m_Transform; }
	
	void PushMatrix() { m_Transform.PushMatrix(); }
	void PopMatrix() { m_Transform.PopMatrix(); }
	void ApplyTransformation(const glm::mat4& m_TransformationMatrix) { *m_Transform.GetModelMatrix() *= m_TransformationMatrix; }

	virtual void mousePressEvent(QMouseEvent* p_Event);
	virtual void mouseReleaseEvent(QMouseEvent* p_Event);
	virtual void mouseMoveEvent(QMouseEvent* p_Event);

	GETSET(glm::mat4*, Projection)	// Not owned by Scene, double check this can be owned. TODO: PS
	GETSET(glm::mat4*, View)		// Not owned by Scene
	GETSET(Model3D*, CurrentHoverItem)	// Not owned by Scene

public:
    int m_ScreenHeight;
    int m_ScreenWidth;

	std::vector<Model3D*> m_ModelList;
    Transformation3D m_Transform;
    int m_Frame;

    std::vector<QMatrix4x4> m_MatrixVector;
	std::vector<Model3D*> m_FlatList;
};
