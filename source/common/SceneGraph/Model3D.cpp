#include "Model3D.h"
#include<QMouseEvent>
#include<glm/gtx/string_cast.hpp>

Model3D::Model3D(Scene3D *p_Scene, Model3D *p_Parent, const QString &p_Name, SHAPE p_ShapeType, RENDER_SCEHEME_TYPE p_RenderSchemeType)
    : m_Scene(p_Scene)
    , m_Parent(p_Parent)
    , m_ShapeType(p_ShapeType)
	, m_RenderSchemeType(p_RenderSchemeType)
{
    m_Parent ? m_Parent->m_ChildList.append(this) : p_Scene->AddModel(this);
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
	glm::vec4 posStart((0 * m_Dimension.x), (0 * m_Dimension.y), 0.0, 1.0);
	glm::vec4 posStartResult = m_TransformedModel * posStart;

	glm::vec4 posEnd((m_Dimension.x), (m_Dimension.y), 0.0, 1.0);
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
	glm::vec4 posStart((0 * m_Dimension.x), (0 * m_Dimension.y), 0.0, 1.0);
	glm::vec4 posStartResult = /*GetParentsTransformation(GetParent()) **/ m_TransformedModel * posStart;

	glm::vec4 posEnd((m_Dimension.x), (m_Dimension.y), 0.0, 1.0);
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

void Model3D::Update()
{
    m_Scene->PushMatrix();
    m_Scene->ApplyTransformation(m_Model);

    m_TransformedModel = *m_Scene->m_Transform.GetModelMatrix();

    Q_FOREACH(Model3D* child, m_ChildList)
    {
        assert(child);
        child->Update();
    }

    m_Scene->PopMatrix();
}

void Model3D::Rectangle(float p_X, float p_Y, float p_Width, float p_Height, float p_ZOrder/*=0*/)
{
	Translate(p_X, p_Y, p_ZOrder);

    m_Position.x = p_X;
    m_Position.y = p_Y;
    m_Position.z = p_ZOrder;

    m_Dimension.x = p_Width;
	m_Dimension.y = p_Height;
}

void Model3D::SetZOrder(float p_ZOrder)
{
    m_Position.z = p_ZOrder;

    Reset();
    Translate(m_Position.x, m_Position.y, m_Position.z);
}

void Model3D::GatherFlatList()
{
    m_Scene->m_FlatList.push_back(this);

    Q_FOREACH(Model3D* child, m_ChildList)
    {
        assert(child);
        child->GatherFlatList();
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////
#include "../Chapter5/Ch5_03_UI_Shapes/source/Cube.h"
//#include "../Chapter6/Ch6_02_DrawingMultipleObjects/source/Rect.h"

ProgressBar::ProgressBar(Scene3D* p_Scene, Model3D* p_Parent, const QString& p_Name, SHAPE p_ShapeType)
	: Model3D(p_Scene, p_Parent, p_Name, p_ShapeType)
{
    RectangleModel* background = new RectangleModel(NULL, m_Scene, this, "Rectangle 1", SHAPE_RECTANGLE);
	background->Rectangle(10, 10, 400, 50);
	background->SetColor(glm::vec4(0.6, 01.0, 0.0, 1.0));
	background->SetDefaultColor(glm::vec4(0.42, 0.65, 0.60, 1.0));
    //background->SetDrawType(RectangleModel::OUTLINE);

    RectangleModel* bar = new RectangleModel(NULL, m_Scene, background, "Bar", SHAPE_RECTANGLE);
    bar->Rectangle(0, (background->GetDimension().y * 0.25f), 400, 25);
	bar->SetColor(glm::vec4(0.6, 0.52, 0.320, 1.0));
	bar->SetDefaultColor(glm::vec4(0.42, 0.15, 0.60, 1.0));
    bar->SetDrawType(RectangleModel::OUTLINE);

    progressIndicator = new RectangleModel(NULL, m_Scene, bar, "ProgressIndicator", SHAPE_RECTANGLE);
    progressIndicator->Rectangle(0, 0, 20, background->GetDimension().y);
	progressIndicator->Translate(0, -(background->GetDimension().y * 0.25f), 0);
	progressIndicator->SetColor(glm::vec4(0.1, 0.52, 0.320, 1.0));
	progressIndicator->SetDefaultColor(glm::vec4(0.2, 0.15, 0.60, 1.0));
    progressIndicator->SetDrawType(RectangleModel::OUTLINE);
}

bool ProgressBar::mouseMoveEvent(QMouseEvent* p_Event)
{
    if (progressIndicator->mouseMoveEvent(p_Event))
    {
        progressIndicator->Translate(p_Event->x(), 0.0, 0.0);
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////

AudioMixerItem::AudioMixerItem(Scene3D* p_Scene, Model3D* p_Parent, const QString& p_Name, glm::vec2 p_TopLeftPos, glm::vec2 p_Dim, SHAPE p_ShapeType)
    : Model3D(p_Scene, p_Parent, p_Name, p_ShapeType)
{
	Rectangle(p_TopLeftPos.x, p_TopLeftPos.y, p_Dim.x, p_Dim.y);

	Model3D* background = new RectangleModel(NULL, m_Scene, this, "Audio Mixer Background", SHAPE_RECTANGLE);
	background->Rectangle(0, 0, p_Dim.x, p_Dim.y);
	background->SetColor(glm::vec4(47.0f / 255.0f, 48.0f / 255.0f, 44.0f / 255.0f, 1.0));
	background->SetDefaultColor(glm::vec4(47.0f / 255.0f, 48.0f / 255.0f, 44.0f / 255.0f, 1.0));

	bool isActiveTrack = true;
	const int activeIndicatorWidth = 7;
	const int activeTrackIndicatorTopMargin = 5.0;
	const int activeTrackIndicatorTopMarginLeftMargin = 4.0;
	Model3D* activeTrackIndicator = new RectangleModel(NULL, m_Scene, background, "Active Track Indicator", SHAPE_RECTANGLE);
	activeTrackIndicator->Rectangle(activeTrackIndicatorTopMarginLeftMargin, activeTrackIndicatorTopMargin, p_Dim.x - (5 * activeTrackIndicatorTopMarginLeftMargin), activeIndicatorWidth);
	activeTrackIndicator->SetColor(glm::vec4(67.0f / 255.0f, 139.0f / 255.0f, 98.0f / 255.0f, 1.0));
	activeTrackIndicator->SetDefaultColor(glm::vec4(67.0f / 255.0f, 139.0f / 255.0f, 98.0f / 255.0f, 1.0));
	
    static int cnt = 0;
	const int formatType = 7;
	const int channelTopMargin = activeTrackIndicatorTopMargin + 15.0;
	const int channelLeftMargin = 4.0;
	const int channelWidth = (p_Dim.x / formatType)/2;
	for (int i = 0; i < formatType; i++)
	{
		Model3D* channelBackground = new RectangleModel(NULL, m_Scene, background, "Channel", SHAPE_RECTANGLE);
		channelBackground->Rectangle((i * channelWidth) + channelLeftMargin, channelTopMargin, ((i == (formatType - 1)) ? 2 : 0) + channelWidth, p_Dim.y - channelTopMargin - 5.0);
		channelBackground->SetColor(glm::vec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0));
		channelBackground->SetDefaultColor(glm::vec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0));

		Model3D* channel = new RectangleModel(NULL, m_Scene, channelBackground, "Channel", SHAPE_RECTANGLE);
		channel->Rectangle(2, 2, channelWidth - 2, p_Dim.y - channelTopMargin - 5.0 - 4);
		channel->SetColor(glm::vec4(47.0f / 255.0f, 48.0f / 255.0f, 44.0f / 255.0f, 1.0));
		channel->SetDefaultColor(glm::vec4(47.0f / 255.0f, 48.0f / 255.0f, 44.0f / 255.0f, 1.0));
		
		glm::vec4 red(246.0 / 255.0f, 24.0 / 255.0f, 39.0f / 255.0f, 1.0);
		glm::vec4 yellow(226.0 / 255.0f, 208.0 / 255.0f, 4.0f / 255.0f, 1.0);
		glm::vec4 green(29.0 / 255.0f, 148.0 / 255.0f, 56.0f / 255.0f, 1.0);
		const int totalRangeIndicator = channel->GetRefDimension().y / 4;
		const int redIndicatorRange = totalRangeIndicator * 0.05;
		const int yellowIndicatorRange = totalRangeIndicator * 0.20;
		for (int j = 0; j < totalRangeIndicator; j++)
		{
			Model3D* levelIndicator = new RectangleModel(NULL, m_Scene, channel, "Channel", SHAPE_RECTANGLE);
			levelIndicator->Rectangle(2, j * 4, channelWidth - 4.0, 2.0);

			const glm::vec4 color = (j <= redIndicatorRange) ? red : ((j <= yellowIndicatorRange) ? yellow : green);
			levelIndicator->SetColor(color);
			levelIndicator->SetDefaultColor(color);
            cnt++;
		}
	}
}

//bool AudioMixerItem::mouseMoveEvent(QMouseEvent* p_Event)
//{
//    return false;
//}
