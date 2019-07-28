#include "Scene.h"
#include "Node.h"
#include "../../common/VulkanApp.h"

#include <QMouseEvent>

Scene::Scene(AbstractApp* p_Application, const QString& p_Name)
    : m_Application(p_Application)
    , m_Name(p_Name)
    , m_Frame(0)
    , m_ScreenWidth(800)
    , m_ScreenHeight(600)
    , m_CurrentHoverItem(NULL)
{
}

Scene::~Scene()
{
    std::map<SHAPE, RenderSchemeFactory*>::iterator itSRST = m_ShapeRenderSchemeTypeMap.begin();

    while (itSRST != m_ShapeRenderSchemeTypeMap.end())
    {
        delete itSRST->second;

        ++itSRST;
    }

    foreach (Node* currentModel, m_NodeList)
    {
        delete currentModel;
    }

    std::cout << "\n Scene " << m_Name.toStdString() << " Destructed.\n";
}

void Scene::Setup()
{
    GatherFlatNodeList(); // Assuming all nodes are added into the scenes by now

    foreach (Node* currentModel, m_NodeList)
    {
        currentModel->Setup();
    }

    foreach(Node* currentModel, m_FlatList)
    {
        RenderSchemeFactory* renderSchemeFactory = GetRenderSchemeFactory(currentModel); // Populate factories
        if (!renderSchemeFactory) continue;

        renderSchemeFactory->UpdateNodeList(currentModel);
    }

    std::map<SHAPE, RenderSchemeFactory*>::iterator itSRST = m_ShapeRenderSchemeTypeMap.begin();
    while (itSRST != m_ShapeRenderSchemeTypeMap.end())
    {
        m_RenderSchemeFactorySet.insert(itSRST->second);

        itSRST++;
    }

    assert(m_RenderSchemeFactorySet.size());
    foreach(RenderSchemeFactory* currentModelFactory, m_RenderSchemeFactorySet)
    {
        currentModelFactory->Setup();
    }
}

void Scene::SetupRenderFactory(VkCommandBuffer& p_CommandBuffer)
{
    foreach(RenderSchemeFactory* currentModelFactory, m_RenderSchemeFactorySet)
    {
        currentModelFactory->Render(p_CommandBuffer);
    }
}

void Scene::Update()
{
    foreach(RenderSchemeFactory* currentModelFactory, m_RenderSchemeFactorySet)
    {
        currentModelFactory->SetRefProjectViewMatrix(*m_Transform.GetProjectionMatrix() * *m_Transform.GetViewMatrix());
    }

    foreach (Node* item, m_NodeList)
    {
        assert(item);

        item->Update();
    }

    foreach(RenderSchemeFactory* currentModelFactory, m_RenderSchemeFactorySet)
    {
        currentModelFactory->Update();
    }
}

void Scene::Render(VkCommandBuffer& p_CommandBuffer)
{
    foreach(RenderSchemeFactory* currentModelFactory, m_RenderSchemeFactorySet)
    {
        currentModelFactory->Render(p_CommandBuffer);
    }
}

void Scene::GatherFlatNodeList()
{
    m_FlatList.clear();

    foreach (Node* item, m_NodeList)
    {
        assert(item);

        item->GatherFlatNodeList();
    }
}

void Scene::AddItem(Node* p_Item)
{
    if (p_Item && !p_Item->GetParent())
    {
        m_NodeList.push_back(p_Item);
    }
}

void Scene::RemoveItem(Node* p_Item)
{
    while (true)
    {
        auto result = std::find(std::begin(m_NodeList), std::end(m_NodeList), p_Item);
        if (result == std::end(m_NodeList)) break;

        m_NodeList.erase(result);
    }
}

void Scene::Resize(VkCommandBuffer& p_CommandBuffer, int p_Width, int p_Height)
{
    m_ScreenWidth = p_Width;
    m_ScreenHeight = p_Height;

    foreach(RenderSchemeFactory* currentModelFactory, m_RenderSchemeFactorySet)
    {
        currentModelFactory->ResizeWindow(p_CommandBuffer);
    }
}

// Default implementation, extend this function as per requirement in your function
void Scene::SetUpProjection()
{
    m_Transform.SetMatrixMode(Transformation::PROJECTION_MATRIX);
    m_Transform.LoadIdentity();
    m_Transform.Ortho(0.0f, static_cast<float>(m_ScreenWidth), 0.0f, static_cast<float>(m_ScreenHeight), -1.0f, 1.0f);

    m_Transform.SetMatrixMode(Transformation::VIEW_MATRIX);
    m_Transform.LoadIdentity();

    m_Transform.SetMatrixMode(Transformation::MODEL_MATRIX);
    m_Transform.LoadIdentity();
}

void Scene::mousePressEvent(QMouseEvent* p_Event)
{
    foreach (Node* item, m_NodeList)
    {
        assert(item);

        item->mousePressEvent(p_Event);
    }
}

void Scene::mouseReleaseEvent(QMouseEvent* p_Event)
{
    foreach (Node* item, m_NodeList)
    {
        assert(item);

        item->mouseReleaseEvent(p_Event);
    }
}

void Scene::mouseMoveEvent(QMouseEvent* p_Event)
{
    static Node* oldModelItem = NULL;
    for (int i = m_NodeList.size() - 1; i >= 0; i--)
    {
        Node* item = m_NodeList.at(i);
        assert(item);

        item->mouseMoveEvent(p_Event);
        if (p_Event->isAccepted())
        {
            Node* currentModel = GetCurrentHoverItem();
            if (oldModelItem && oldModelItem != currentModel)
            {
                oldModelItem->SetColor(oldModelItem->GetDefaultColor());
            }

            currentModel->SetColor(glm::vec4(1.0, 1.0, 0.3, 0.5));
            oldModelItem = GetCurrentHoverItem();
            return;
        }
    }

    if (oldModelItem)
    {
        oldModelItem->SetColor(oldModelItem->GetDefaultColor());
    }
}

RenderSchemeFactory* Scene::GetRenderSchemeFactory(Node* p_Item)
{
    const SHAPE shapeType = p_Item->GetShapeType();
    if ((shapeType <= SHAPE_NONE) && (shapeType >= SHAPE_COUNT)) return NULL;

    std::map<SHAPE, RenderSchemeFactory*>::iterator it = m_ShapeRenderSchemeTypeMap.find(shapeType);
    if (it != m_ShapeRenderSchemeTypeMap.end())
    {
        return it->second;
    }

    RenderSchemeFactory* renderFactory = p_Item->GetRenderSchemeFactory();
    if (renderFactory)
    {
        (m_ShapeRenderSchemeTypeMap)[shapeType] = renderFactory;
    }

    return renderFactory;
}

void Scene::AppendToFlatNodeList(Node* p_Item)
{
    m_FlatList.push_back(p_Item);
}
