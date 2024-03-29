#include "Model3D.h"
#include<QMouseEvent>
#include<glm/gtx/string_cast.hpp>

Model3D::Model3D(Scene3D *p_Scene, Model3D* p_Parent, const BoundingRegion& p_BoundedRegion, const QString& p_Name, SHAPE p_ShapeType, RENDER_SCEHEME_TYPE p_RenderSchemeType)
    : m_Scene(p_Scene)
    , m_Parent(p_Parent)
    , m_ShapeType(p_ShapeType)
	, m_RenderSchemeType(p_RenderSchemeType)
    , m_BoundedRegion(p_BoundedRegion)
{
    m_Parent ? m_Parent->m_ChildList.append(this) : p_Scene->AddModel(this);

    // Todo: We can directly use the translate as the m_BoundedRegion is already set
    SetGeometry(m_BoundedRegion.m_Position.x, m_BoundedRegion.m_Position.y, m_BoundedRegion.m_Dimension.x, m_BoundedRegion.m_Dimension.y, m_BoundedRegion.m_Position.z);
}

void Model3D::Setup()
{
    foreach(Model3D* currentModel, m_ChildList)
    {
        Model3D* model = /*dynamic_cast<Model3D*>*/(currentModel);

        if (!model) continue;

        model->Setup();
    }
}

void Model3D::mousePressEvent(QMouseEvent* p_Event)
{
	glm::vec4 posStart((0 * m_BoundedRegion.m_Dimension.x), (0 * m_BoundedRegion.m_Dimension.y), 0.0, 1.0);
	glm::vec4 posStartResult = m_TransformedModel * posStart;

	glm::vec4 posEnd((m_BoundedRegion.m_Dimension.x), (m_BoundedRegion.m_Dimension.y), 0.0, 1.0);
	glm::vec4 posEndResult = m_TransformedModel * posEnd;

	cout << "\n##### mousePressEventS" << glm::to_string(posStartResult);// << posEndResult;
	cout << "\n##### mousePressEventE" << glm::to_string(posEndResult);// << posEndResult;

	QRect rect(QPoint(posStartResult.x, posStartResult.y), QPoint(posEndResult.x, posEndResult.y));
	if (rect.contains(p_Event->x(), p_Event->y()))
		cout << "\n***************";
	

	foreach(Model3D* item, m_ChildList)
	{
		assert(item);

		item->mousePressEvent(p_Event);
	}
}

void Model3D::mouseReleaseEvent(QMouseEvent* p_Event)
{
	//cout << "\n##### mouseReleaseEvent";
	foreach(Model3D* item, m_ChildList)
	{
		assert(item);

		item->mouseReleaseEvent(p_Event);
	}
}

bool Model3D::mouseMoveEvent(QMouseEvent* p_Event)
{
	glm::vec4 posStart((0 * m_BoundedRegion.m_Dimension.x), (0 * m_BoundedRegion.m_Dimension.y), 0.0, 1.0);
	glm::vec4 posStartResult = /*GetParentsTransformation(GetParent()) **/ m_TransformedModel * posStart;

	glm::vec4 posEnd((m_BoundedRegion.m_Dimension.x), (m_BoundedRegion.m_Dimension.y), 0.0, 1.0);
	glm::vec4 posEndResult = /*GetParentsTransformation(GetParent()) **/ m_TransformedModel * posEnd;

	QRect rect(QPoint(posStartResult.x, posStartResult.y), QPoint(posEndResult.x, posEndResult.y));
	if (rect.contains(p_Event->x(), p_Event->y()))
	{
		m_Scene->SetCurrentHoverItem(this);

		//cout << "\n##### mouseMoveEvent";
		for (int i = m_ChildList.size() - 1; i >= 0; i--)
		{
			Model3D* item = m_ChildList.at(i);
			assert(item);

			if (item->mouseMoveEvent(p_Event))
			{
				return true;
			}
		}

		return true;
	}

	return false;
}

void Model3D::Update(Model3D* p_Item)
{
    m_Scene->PushMatrix();
    if (p_Item)
    {
        m_Scene->ApplyTransformation(p_Item->GetParentsTransformation(GetParent()) * m_Model); // This retrives all the transformation from the parent
    }
    else
    {
        m_Scene->ApplyTransformation(m_Model);
    }

    m_TransformedModel = *m_Scene->m_Transform.GetModelMatrix();
    
    QList<Model3D*>& childList = (p_Item ? p_Item->m_ChildList : m_ChildList);
    Q_FOREACH(Model3D* child, childList )
    {
        assert(child);
        child->Update();
    }

    m_Scene->PopMatrix();
}

void Model3D::SetGeometry(float p_X, float p_Y, float p_Width, float p_Height, float p_ZOrder/*=0*/)
{
    Translate(p_X, p_Y, p_ZOrder);

    m_BoundedRegion.m_Position.x = p_X;
    m_BoundedRegion.m_Position.y = p_Y;
    m_BoundedRegion.m_Position.z = p_ZOrder;

    m_BoundedRegion.m_Dimension.x = p_Width;
    m_BoundedRegion.m_Dimension.y = p_Height;
}

void Model3D::SetZOrder(float p_ZOrder)
{
    m_BoundedRegion.m_Position.z = p_ZOrder;

    Reset();
    Translate(m_BoundedRegion.m_Position.x, m_BoundedRegion.m_Position.y, m_BoundedRegion.m_Position.z);
}

void Model3D::SetPosition(float p_X, float p_Y)
{
    glm::vec4 posStart((0 * m_BoundedRegion.m_Dimension.x), (0 * m_BoundedRegion.m_Dimension.y), 0.0, 1.0);
    glm::vec4 posStartResult = m_TransformedModel * posStart;

    glm::vec4 posEnd((m_BoundedRegion.m_Dimension.x), (m_BoundedRegion.m_Dimension.y), 0.0, 1.0);
    glm::vec4 posEndResult = m_TransformedModel * posEnd;

    m_BoundedRegion.m_Position.x = p_X;
    m_BoundedRegion.m_Position.y = p_Y;

    Reset();
    Translate(m_BoundedRegion.m_Position.x, m_BoundedRegion.m_Position.y, m_BoundedRegion.m_Position.z);
    m_TransformedModel = m_Model * GetParentsTransformation(GetParent());
}

void Model3D::GatherFlatModelList()
{
    if (!m_Scene) return;

    m_Scene->m_FlatList.push_back(this);

    Q_FOREACH(Model3D* child, m_ChildList)
    {
        assert(child);
        child->GatherFlatModelList();
    }
}
