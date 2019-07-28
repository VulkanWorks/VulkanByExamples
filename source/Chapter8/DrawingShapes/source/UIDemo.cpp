#include "UIDemo.h"

#include "source/Rectangle/Rect.h"

#if CIRCLE_DEFINED == 1
#include "Circle/Circle.h"
#endif

// Todo: Parminder
// The below flag is temporary, removed once multi-draw added
const bool useInstancing = true;

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

    const float Col = 20;
    const float Row = 20;
    float colWidth = parentColWidth / Col;
    float colHeight = parentColHeight / Row;

    QString name;
    static long itemNum = 0;
    for (int i = 0; i < parentCol; i++)
    {
        for (int j = 0; j < parentRow; j++)
        {
            name = QString::number(++itemNum);
            Node* m_Parent = new Rectangl(p_Scene, NULL, BoundingRegion((i * parentColWidth), (j * parentColHeight), parentColWidth - 2, parentColHeight), name, (useInstancing ? SHAPE::SHAPE_RECTANGLE_MULTIDRAW : SHAPE::SHAPE_RECTANGLE_INSTANCED));
            m_Parent->SetColor(glm::vec4(0.6, 0.2, 0.20, 1.0));
            m_Parent->SetDefaultColor(glm::vec4(0.42, 0.15, 0.60, 1.0));

            for (int k = 0; k < Col; k++)
            {
                for (int l = 0; l < Row; l++)
                {
                    name = QString::number(++itemNum);
                    Rectangl* rect = new Rectangl(p_Scene, m_Parent, BoundingRegion((k * colWidth), (l * colHeight), colWidth, colHeight), name, (useInstancing ? SHAPE::SHAPE_RECTANGLE_MULTIDRAW : SHAPE::SHAPE_RECTANGLE_INSTANCED));
                    rect->SetColor(glm::vec4(l / Col, k / Row, 0.50, 1.0));
                    rect->SetDefaultColor(glm::vec4(0.2, 0.5, 0.50, 1.0));
                    //rect->SetVisible(!(k == l));
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
        AudioMixerItem* m_MixerItem = new AudioMixerItem(p_Scene, NULL, boundedRegion, "Mixer Item 1", SHAPE::SHAPE_CUSTOM);
        m_MixerItem->SetColor(glm::vec4(1.1, 0.2, 0.20, 1.0));
        m_MixerItem->SetDefaultColor(glm::vec4(1.0, 0.15, 0.60, 1.0));
    }
}

void UIDemo::ProgressBarFunc(Scene* p_Scene)
{
    BoundingRegion boundedRegion(200, 400, 400, 100);
    new ProgressBar(p_Scene, NULL, boundedRegion, "", SHAPE::SHAPE_CUSTOM);
}

///////////////////////////////////////////////////////////////////////////////////////////////

ProgressBar::ProgressBar(Scene* p_Scene, Node* p_Parent, const BoundingRegion& p_BoundedRegion, const QString& p_Name, SHAPE p_ShapeType)
    : Node(p_Scene, p_Parent, p_BoundedRegion, p_Name, p_ShapeType)
{
//    this->SetZOrder(20);
    BoundingRegion bgDim(0, 0, 400, 50);
    Rectangl* background = new Rectangl(p_Scene, this, bgDim, "", (useInstancing ? SHAPE::SHAPE_RECTANGLE_MULTIDRAW : SHAPE::SHAPE_RECTANGLE_INSTANCED));
    background->SetColor(glm::vec4(0.6, 1.0, 0.0, 1.0));
    background->SetDefaultColor(glm::vec4(0.42, 0.65, 0.60, 1.0));

    BoundingRegion barDim(0, (bgDim.m_Dimension.y * 0.25f), 400, 25);
    bar = new Rectangl(p_Scene, background, barDim, "Bar", (useInstancing ? SHAPE::SHAPE_RECTANGLE_MULTIDRAW : SHAPE::SHAPE_RECTANGLE_INSTANCED));
    bar->SetColor(glm::vec4(0.6, 0.52, 0.320, 1.0));
    bar->SetDefaultColor(glm::vec4(0.42, 0.15, 0.60, 1.0));

    BoundingRegion piDim(0, 0, 20, bgDim.m_Dimension.y);
    progressIndicator = new Rectangl(p_Scene, bar, piDim, "ProgressIndicator", (useInstancing ? SHAPE::SHAPE_RECTANGLE_MULTIDRAW : SHAPE::SHAPE_RECTANGLE_INSTANCED));
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
    Node* background = new Rectangl(m_Scene, this, BoundingRegion(0, 0, p_BoundedRegion.m_Dimension.x, p_BoundedRegion.m_Dimension.y), "Audio Mixer Background", (useInstancing ? SHAPE::SHAPE_RECTANGLE_MULTIDRAW : SHAPE::SHAPE_RECTANGLE_INSTANCED));
    background->SetColor(glm::vec4(47.0f / 255.0f, 48.0f / 255.0f, 44.0f / 255.0f, 1.0));
    background->SetDefaultColor(glm::vec4(47.0f / 255.0f, 48.0f / 255.0f, 44.0f / 255.0f, 1.0));

    const int activeIndicatorWidth = 7;
    const int activeTrackIndicatorTopMargin = 5.0;
    const int activeTrackIndicatorTopMarginLeftMargin = 4.0;
    Node* activeTrackIndicator = new Rectangl(m_Scene, background, BoundingRegion(activeTrackIndicatorTopMarginLeftMargin, activeTrackIndicatorTopMargin, p_BoundedRegion.m_Dimension.x - (5 * activeTrackIndicatorTopMarginLeftMargin), activeIndicatorWidth), "Active Track Indicator", (useInstancing ? SHAPE::SHAPE_RECTANGLE_MULTIDRAW : SHAPE::SHAPE_RECTANGLE_INSTANCED));
    activeTrackIndicator->SetColor(glm::vec4(67.0f / 255.0f, 139.0f / 255.0f, 98.0f / 255.0f, 1.0));
    activeTrackIndicator->SetDefaultColor(glm::vec4(67.0f / 255.0f, 139.0f / 255.0f, 98.0f / 255.0f, 1.0));

    static int cnt = 0;
    const int formatType = 7;
    const int channelTopMargin = activeTrackIndicatorTopMargin + 15.0;
    const int channelLeftMargin = 4.0;
    const int channelWidth = (p_BoundedRegion.m_Dimension.x / formatType) / 2;
    for (int i = 0; i < formatType; i++)
    {
        Node* channelBackground = new Rectangl(m_Scene, background, BoundingRegion((i * channelWidth) + channelLeftMargin, channelTopMargin, ((i == (formatType - 1)) ? 2 : 0) + channelWidth, p_BoundedRegion.m_Dimension.y - channelTopMargin - 5.0), "Channel", (useInstancing ? SHAPE::SHAPE_RECTANGLE_MULTIDRAW : SHAPE::SHAPE_RECTANGLE_INSTANCED));
        channelBackground->SetColor(glm::vec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0));
        channelBackground->SetDefaultColor(glm::vec4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0));

        Node* channel = new Rectangl(m_Scene, channelBackground, BoundingRegion(2, 2, channelWidth - 2, p_BoundedRegion.m_Dimension.y - channelTopMargin - 5.0 - 4), "Channel", (useInstancing ? SHAPE::SHAPE_RECTANGLE_MULTIDRAW : SHAPE::SHAPE_RECTANGLE_INSTANCED));
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
            Node* levelIndicator = new Rectangl(m_Scene, channel, BoundingRegion(2, j * 4, channelWidth - 4.0, 2.0), "Channel", (useInstancing ? SHAPE::SHAPE_RECTANGLE_MULTIDRAW : SHAPE::SHAPE_RECTANGLE_INSTANCED));

            const glm::vec4 color = (j <= redIndicatorRange) ? red : ((j <= yellowIndicatorRange) ? yellow : green);
            levelIndicator->SetColor(color);
            levelIndicator->SetDefaultColor(color);
            cnt++;
        }
    }
}

TransformationConformTest::TransformationConformTest()
    : m_RectTr1(NULL)
    , m_RectTr2(NULL)
    , m_RectTr3(NULL)
    , m_RectTr4(NULL)
    , m_Rect1(NULL)
    , m_Rect2(NULL)
    , m_Rect3(NULL)
    , m_Rect4(NULL)
#if CIRCLE_DEFINED == 1
    , m_CircleTr5(NULL)
#endif
{

}

void TransformationConformTest::Configure(Scene* p_Scene)
{
    const unsigned int memppoolId = 5;
    m_RectTr1 = new Rectangl(p_Scene, NULL, BoundingRegion(200, 200, 100, 100), "", (useInstancing ? SHAPE::SHAPE_RECTANGLE_MULTIDRAW : SHAPE::SHAPE_RECTANGLE_INSTANCED));
    m_RectTr1->SetColor(glm::vec4(0.6, 0.2, 0.20, 1.0));
    m_RectTr1->SetDefaultColor(glm::vec4(0.42, 0.15, 0.60, 1.0));
    m_RectTr1->SetZOrder(1.1);
    m_RectTr1->SetMemPoolIdx(memppoolId);

    m_RectTr2 = new Rectangl(p_Scene, m_RectTr1, BoundingRegion(100, 100, 50, 50), "", (useInstancing ? SHAPE::SHAPE_RECTANGLE_MULTIDRAW : SHAPE::SHAPE_RECTANGLE_INSTANCED));
    m_RectTr2->SetColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
    m_RectTr2->SetDefaultColor(glm::vec4(0.42, 0.15, 0.60, 1.0));
    m_RectTr2->SetMemPoolIdx(memppoolId);

    m_RectTr3 = new Rectangl(p_Scene, m_RectTr1, BoundingRegion(0, 0, 50, 50), "", (useInstancing ? SHAPE::SHAPE_RECTANGLE_MULTIDRAW : SHAPE::SHAPE_RECTANGLE_INSTANCED));
    m_RectTr3->SetColor(glm::vec4(0.6, 0.0, 1.0, 1.0));
    m_RectTr3->SetDefaultColor(glm::vec4(0.2, 0.55, 0.20, 1.0));
    m_RectTr3->SetMemPoolIdx(memppoolId);

    m_RectTr4 = new Rectangl(p_Scene, m_RectTr1, BoundingRegion(75, -25, 50, 50), "", (useInstancing ? SHAPE::SHAPE_RECTANGLE_MULTIDRAW : SHAPE::SHAPE_RECTANGLE_INSTANCED));
    m_RectTr4->SetZOrder(-10.1);
    m_RectTr4->SetOriginOffset(glm::vec3(25, 25, 0));
    m_RectTr4->SetColor(glm::vec4(0.0, 0.2, 1.0, 1.0));
    m_RectTr4->SetDefaultColor(glm::vec4(0.2, 0.35, 0.30, 1.0));
    m_RectTr4->SetMemPoolIdx(memppoolId);

#if CIRCLE_DEFINED == 1
    m_CircleTr5 = new Circle(p_Scene, m_RectTr1, glm::vec2(0, 0), 50.0f);
    m_CircleTr5->SetOriginOffset(glm::vec3(25, 25, 0));
    m_CircleTr5->SetColor(glm::vec4(0.0, 0.5, 1.0, 1.0));
    m_CircleTr5->SetDefaultColor(glm::vec4(0.62, 0.25, 0.60, 1.0));
    m_CircleTr5->SetZOrder(10.1);
    m_CircleTr5->SetVisible(!false);
    m_CircleTr5->SetName("m_CircleTr5");
    m_CircleTr5->SetMemPoolIdx(memppoolId);
#endif

    {
        float x = 0;
        float y = 0;
        m_Rect1 = new Rectangl(p_Scene, NULL, BoundingRegion(x, y, 100, 100, -1), "Item1", (useInstancing ? SHAPE::SHAPE_RECTANGLE_MULTIDRAW : SHAPE::SHAPE_RECTANGLE_INSTANCED));
        m_Rect1->SetColor(glm::vec4(0.6, 0.2, 0.20, 1.0));
        m_Rect1->SetDefaultColor(glm::vec4(0.42, 0.15, 0.60, 1.0));
        m_Rect1->SetMemPoolIdx(memppoolId);
        x += 50;

        m_Rect2 = new Rectangl(p_Scene, m_Rect1, BoundingRegion(x, y, 100, 100, -1), "Item2", (useInstancing ? SHAPE::SHAPE_RECTANGLE_MULTIDRAW : SHAPE::SHAPE_RECTANGLE_INSTANCED));
        m_Rect2->SetColor(glm::vec4(1.0, 0.2, 0.20, 1.0));
        m_Rect2->SetDefaultColor(glm::vec4(1.42, 0.15, 0.60, 1.0));
        m_Rect2->SetMemPoolIdx(memppoolId);
        x += 50;

        m_Rect3 = new Rectangl(p_Scene, m_Rect1, BoundingRegion(x, y, 100, 100, 10), "Item3", (useInstancing ? SHAPE::SHAPE_RECTANGLE_MULTIDRAW : SHAPE::SHAPE_RECTANGLE_INSTANCED));
        m_Rect3->SetColor(glm::vec4(1.0, 1.2, 0.20, 1.0));
        m_Rect3->SetDefaultColor(glm::vec4(1.42, 1.15, 0.60, 1.0));
        m_Rect3->SetMemPoolIdx(memppoolId);
        x += 50;

        m_Rect4 = new Rectangl(p_Scene, m_Rect1, BoundingRegion(x, y, 100, 100, -1000), "Item4", (useInstancing ? SHAPE::SHAPE_RECTANGLE_MULTIDRAW : SHAPE::SHAPE_RECTANGLE_INSTANCED));
        m_Rect4->SetColor(glm::vec4(1.0, 1.2, 1.0, 1.0));
        m_Rect4->SetDefaultColor(glm::vec4(1., 0.5, 0.60, 1.0));
        m_Rect4->SetMemPoolIdx(memppoolId);
        x += 50;
    }
}

void TransformationConformTest::Update()
{
    if (!m_RectTr1) return;

    static float rot = 0.0;
    {
        m_RectTr1->ResetPosition();
        m_RectTr1->Rotate(.001, 0.0, 0.0, 1.0);

        m_RectTr2->ResetPosition();
        m_RectTr2->Rotate(rot += .1, 0.0, 0.0, 1.0);

        m_RectTr3->Rotate(.003, 0.0, 0.0, 1.0);
        m_RectTr4->Rotate(.003, 0.0, 0.0, 1.0);
    }
    {
        m_Rect1->Rotate(.003, 0.0, 0.0, 1.0);
        m_Rect2->Rotate(.003, 0.0, 0.0, 1.0);
        m_Rect3->Rotate(.003, 0.0, 0.0, 1.0);
        m_Rect4->Rotate(.003, 0.0, 0.0, 1.0);
    }
}
