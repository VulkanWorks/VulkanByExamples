#include "UIDemo.h"

UIDemo::UIDemo()
{
}

UIDemo::~UIDemo()
{
}

void UIDemo::Grid(Scene* p_Scene, int p_Width, int p_Height)
{
	float parentCol = 2;
	float parentRow = 2;
	float parentColWidth = p_Width / parentCol;
	float parentColHeight = p_Height / parentRow;

	const float Col = 2;
	const float Row = 1;
	float colWidth = parentColWidth / Col;
	float colHeight = parentColHeight / Row;

	for (int i = 0; i < parentCol; i++)
	{
		for (int j = 0; j < parentRow; j++)
		{
            Node* m_Parent = new Rectangl(p_Scene, NULL, BoundingRegion((i * parentColWidth), (j * parentColHeight), parentColWidth - 2, parentColHeight));
			m_Parent->SetColor(glm::vec4(0.6, 0.2, 0.20, 1.0));
			m_Parent->SetDefaultColor(glm::vec4(0.42, 0.15, 0.60, 1.0));

			for (int k = 0; k < Col; k++)
			{
				for (int l = 0; l < Row; l++)
				{
					Rectangl* m_Cube1 = new Rectangl(p_Scene, m_Parent, BoundingRegion((k * colWidth), (l * colHeight), colWidth, colHeight));
					m_Cube1->SetColor(glm::vec4(0.2, 0.5, 0.50, 1.0));
					m_Cube1->SetDefaultColor(glm::vec4(0.2, 0.5, 0.50, 1.0));
				}
			}
		}
	}
}

void UIDemo::MixerView(Scene* p_Scene, int p_Width, int p_Height)
{
    const float mixerPanelWidth = p_Width;
    const float mixerPanelHeight = p_Height;

    const float mixerWidth = 100;
    const int numberOfMixers = mixerPanelWidth / mixerWidth;

    for (int i = 0; i < numberOfMixers; i++)
    {
        BoundingRegion boundedRegion((i * mixerWidth), 0, mixerWidth, mixerPanelHeight);
        AudioMixerItem* m_MixerItem = new AudioMixerItem(p_Scene, NULL, boundedRegion, "Mixer Item 1", SHAPE_CUSTOM);
        m_MixerItem->SetColor(glm::vec4(1.1, 0.2, 0.20, 1.0));
        m_MixerItem->SetDefaultColor(glm::vec4(1.0, 0.15, 0.60, 1.0));
    }
}

void UIDemo::ProgressBarFunc(Scene* p_Scene)
{
    BoundingRegion boundedRegion(200, 400, 400, 100);
    new ProgressBar(p_Scene, NULL, boundedRegion, "", SHAPE_CUSTOM);
}

///////////////////////////////////////////////////////////////////////////////////////////////

ProgressBar::ProgressBar(Scene* p_Scene, Node* p_Parent, const BoundingRegion& p_BoundedRegion, const QString& p_Name, SHAPE p_ShapeType)
: Node(p_Scene, p_Parent, p_BoundedRegion, p_Name, p_ShapeType)
{
    BoundingRegion bgDim(10, 10, 400, 50);
    Node* background = new Rectangl(p_Scene, this, bgDim);
    background->SetColor(glm::vec4(0.6, 1.0, 0.0, 1.0));
    background->SetDefaultColor(glm::vec4(0.42, 0.65, 0.60, 1.0));

    BoundingRegion barDim(0, (bgDim.m_Dimension.y * 0.25f), 400, 25);
    bar = new Rectangl(p_Scene, background, barDim, "Bar");
    bar->SetColor(glm::vec4(0.6, 0.52, 0.320, 1.0));
    bar->SetDefaultColor(glm::vec4(0.42, 0.15, 0.60, 1.0));

    BoundingRegion piDim(0, 0, 20, bgDim.m_Dimension.y);
    progressIndicator = new Rectangl(p_Scene, bar, piDim, "ProgressIndicator");
    progressIndicator->Translate(0, -((bgDim.m_Dimension.y * 0.25f) * 0.25f), 0);
    progressIndicator->SetColor(glm::vec4(0.1, 0.52, 0.320, 1.0));
    progressIndicator->SetDefaultColor(glm::vec4(0.2, 0.15, 0.60, 1.0));
}

void ProgressBar::mousePressEvent(QMouseEvent *p_Event)
{

}

void ProgressBar::mouseMoveEvent(QMouseEvent* p_Event)
{
    bar->mouseMoveEvent(p_Event);
    {
        //progressIndicator->Translate(p_Event->x(), 0.0, 0.0);
        //progressIndicator->SetPosition(p_Event->x(),  GetPosition().y());
        //progressIndicator->SetPosition(p_Event->x(), progressIndicator->GetPosition().y);
        //return true;
    }
}

void ProgressBar::mouseReleaseEvent(QMouseEvent *p_Event)
{

}

void ProgressBar::EmitValueChanged(int p_NewValue)
{

}

////////////////////////////////////////////////////////////////////////////////////////////

AudioMixerItem::AudioMixerItem(Scene* p_Scene, Node* p_Parent, const BoundingRegion& p_BoundedRegion, const QString& p_Name, SHAPE p_ShapeType)
    : Node(p_Scene, p_Parent, p_BoundedRegion, p_Name, p_ShapeType)
{
    Node* background = new Rectangl(m_Scene, this, BoundingRegion(0, 0, p_BoundedRegion.m_Dimension.x, p_BoundedRegion.m_Dimension.y));
    background->SetColor(glm::vec4(47.0f / 255.0f, 48.0f / 255.0f, 44.0f / 255.0f, 1.0));
    background->SetDefaultColor(glm::vec4(47.0f / 255.0f, 48.0f / 255.0f, 44.0f / 255.0f, 1.0));

    bool isActiveTrack = true;
    const int activeIndicatorWidth = 7;
    const int activeTrackIndicatorTopMargin = 5.0;
    const int activeTrackIndicatorTopMarginLeftMargin = 4.0;
    Node* activeTrackIndicator = new Rectangl(m_Scene, background, BoundingRegion(activeTrackIndicatorTopMarginLeftMargin, activeTrackIndicatorTopMargin, p_BoundedRegion.m_Dimension.x - (5 * activeTrackIndicatorTopMarginLeftMargin), activeIndicatorWidth));
    activeTrackIndicator->SetColor(glm::vec4(67.0f / 255.0f, 139.0f / 255.0f, 98.0f / 255.0f, 1.0));
    activeTrackIndicator->SetDefaultColor(glm::vec4(67.0f / 255.0f, 139.0f / 255.0f, 98.0f / 255.0f, 1.0));

    static int cnt = 0;
    const int formatType = 7;
    const int channelTopMargin = activeTrackIndicatorTopMargin + 15.0;
    const int channelLeftMargin = 4.0;
    const int channelWidth = (p_BoundedRegion.m_Dimension.x / formatType) / 2;
    for (int i = 0; i < formatType; i++)
    {
        Node* channelBackground = new Rectangl(m_Scene, background, BoundingRegion((i * channelWidth) + channelLeftMargin, channelTopMargin, ((i == (formatType - 1)) ? 2 : 0) + channelWidth, p_BoundedRegion.m_Dimension.y - channelTopMargin - 5.0));
        channelBackground->SetColor(glm::vec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0));
        channelBackground->SetDefaultColor(glm::vec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0));

        Node* channel = new Rectangl(m_Scene, channelBackground, BoundingRegion(2, 2, channelWidth - 2, p_BoundedRegion.m_Dimension.y - channelTopMargin - 5.0 - 4));
        channel->SetColor(glm::vec4(47.0f / 255.0f, 48.0f / 255.0f, 44.0f / 255.0f, 1.0));
        channel->SetDefaultColor(glm::vec4(47.0f / 255.0f, 48.0f / 255.0f, 44.0f / 255.0f, 1.0));

        glm::vec4 red(246.0 / 255.0f, 24.0 / 255.0f, 39.0f / 255.0f, 1.0);
        glm::vec4 yellow(226.0 / 255.0f, 208.0 / 255.0f, 4.0f / 255.0f, 1.0);
        glm::vec4 green(29.0 / 255.0f, 148.0 / 255.0f, 56.0f / 255.0f, 1.0);
        const int totalRangeIndicator = channel->GetBoundedRegion().m_Dimension.y / 4;
        const int redIndicatorRange = totalRangeIndicator * 0.05;
        const int yellowIndicatorRange = totalRangeIndicator * 0.20;
        for (int j = 0; j < totalRangeIndicator; j++)
        {
            Node* levelIndicator = new Rectangl(m_Scene, channel, BoundingRegion(2, j * 4, channelWidth - 4.0, 2.0));

            const glm::vec4 color = (j <= redIndicatorRange) ? red : ((j <= yellowIndicatorRange) ? yellow : green);
            levelIndicator->SetColor(color);
            levelIndicator->SetDefaultColor(color);
            cnt++;
        }
    }
}
