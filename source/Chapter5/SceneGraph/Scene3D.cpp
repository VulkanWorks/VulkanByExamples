#include "Scene3D.h"
#include "Model3D.h"

#include <QMouseEvent> 
extern bool isDirty;

Scene3D::Scene3D(const QString& p_Name)
	: m_Frame(0)
	, m_ScreenWidth(800)
	, m_ScreenHeight(600)
	, m_CurrentHoverItem(NULL)
	, m_Projection(NULL)	// Not owned by Scene, double check this can be owned. TODO: PS
	, m_View(NULL)		// Not owned by Scene
{
}

Scene3D::~Scene3D()
{
    foreach (Model3D* currentModel, m_ModelList)
    {
        delete currentModel;
    }
}

void Scene3D::Setup()
{
	foreach (Model3D* currentModel, m_ModelList)
	{
		currentModel->Setup();
	}
}

void Scene3D::Update()
{
	m_FlatList.clear();

    foreach (Model3D* item, m_ModelList)
    {
        assert(item);

        item->Update();
    }
}

void Scene3D::AddModel(Model3D* p_Model)
{
    if (p_Model && !p_Model->GetParent())
    {
        m_ModelList.push_back(p_Model);
    }
}

void Scene3D::RemoveModel(Model3D* p_Model)
{
	while (1)
	{
		auto result = std::find(std::begin(m_ModelList), std::end(m_ModelList), p_Model);
		if (result == std::end(m_ModelList)) break;
		
		m_ModelList.erase(result);
	} 
}

void Scene3D::Resize(int p_Width, int p_Height)
{
    m_ScreenWidth = p_Width;
    m_ScreenHeight = p_Height;
}

void Scene3D::SetUpProjection()
{
    m_Transform.SetMatrixMode(Transformation3D::PROJECTION_MATRIX);
    m_Transform.LoadIdentity();

	m_Transform.SetPerspective(45.0f, float(m_ScreenWidth)/m_ScreenHeight, 0.10f, 100.0f);

    m_Transform.SetMatrixMode(Transformation3D::VIEW_MATRIX);
	glm::vec3 eye(10.0, 10.0, 10.0); 
	glm::vec3 center(0.0, 0.0, 0.0); 
	glm::vec3 up(0.0, 1.0, 0.0);
	m_Transform.LoadIdentity(); 
	m_Transform.LookAt(&eye, &center, &up);	

    m_Transform.SetMatrixMode(Transformation3D::MODEL_MATRIX);
    m_Transform.LoadIdentity();
}

void Scene3D::mousePressEvent(QMouseEvent* p_Event)
{
	foreach (Model3D* item, m_ModelList)
	{
		assert(item);

		item->mousePressEvent(p_Event);
	}
}

void Scene3D::mouseReleaseEvent(QMouseEvent* p_Event)
{
	foreach (Model3D* item, m_ModelList)
	{
		assert(item);

		item->mouseReleaseEvent(p_Event);
	}
}

void Scene3D::mouseMoveEvent(QMouseEvent* p_Event)
{
	Model3D* oldModelItem = GetCurrentHoverItem();
	for (int i = m_ModelList.size() - 1; i >= 0; i--)
	{
		Model3D* item = m_ModelList.at(i);
		assert(item);

		if (item->mouseMoveEvent(p_Event))
		{
			Model3D* currentModel = GetCurrentHoverItem();
			if (oldModelItem && oldModelItem != currentModel)
			{
				oldModelItem->SetColor(oldModelItem->GetDefaultColor());
			}

			currentModel->SetColor(glm::vec4(1.0, 1.0, 0.3, 1.0));
			isDirty = true;
			return;
		}
	}

	if (oldModelItem)
	{
		oldModelItem->SetColor(oldModelItem->GetDefaultColor());
	}
}