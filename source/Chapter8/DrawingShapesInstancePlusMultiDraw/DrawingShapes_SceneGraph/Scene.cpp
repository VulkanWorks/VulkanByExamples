#include "Scene.h"
#include "Node.h"

#include "../../common/VulkanApp.h" // Not a good design to include vulkan app here: Todo move AbstractApp 
#include <QMouseEvent> 
#include <QMessageBox>

#define DIRTY_UPDATE_IMPLEMENTED 1

Scene::Scene(AbstractApp* p_Application, const QString& p_Name)
    : m_Application(p_Application)
    , m_Name(p_Name)
    , m_Frame(0)
    , m_EarlyDepthTest(false)
    , m_ScreenWidth(1200)
    , m_ScreenHeight(800)
    , m_CurrentHoverItem(NULL)
    , m_DirtyType(SCENE_DIRTY_TYPE::ALL)
{
}

Scene::~Scene()
{
//    std::map<SHAPE, RenderSchemeFactory*>::iterator itSRST = m_ShapeRenderSchemeTypeMap.begin();
    std::multimap<SHAPE, RenderSchemeFactory*>::iterator itSRST = m_ShapeRenderSchemeTypeMap.begin();

    while (itSRST != m_ShapeRenderSchemeTypeMap.end())
    {
        delete itSRST->second;

        ++itSRST;
    }

    foreach (Node* currentNode, m_RootDrawableList)
    {
        delete currentNode;
    }

    std::cout << "\n Scene " << m_Name.toStdString() << " Destructed.\n";
}

void Scene::Setup()
{
    GatherFlatNodeList(); // Assuming all nodes are added into the scenes by now

    foreach (Node* currentModel, m_RootDrawableList)
    {
        currentModel->Setup();
    }

    foreach (Node* currentModel, m_FlatList)
    {
        RenderSchemeFactory* renderSchemeFactory = GetRenderSchemeFactory(currentModel); // Populate factories
        if (!renderSchemeFactory) continue;

        renderSchemeFactory->UpdateNodeList(currentModel);
    }

//    std::map<SHAPE, RenderSchemeFactory*>::iterator itSRST = m_ShapeRenderSchemeTypeMap.begin();
    std::multimap<SHAPE, RenderSchemeFactory*>::iterator itSRST = m_ShapeRenderSchemeTypeMap.begin();
    while (itSRST != m_ShapeRenderSchemeTypeMap.end())
    {
        m_RenderSchemeFactorySet.insert(itSRST->second);

        itSRST++;
    }

    //assert(m_RenderSchemeFactorySet.size()); // Commented because painter engine adds the node on fly
    foreach(RenderSchemeFactory* currentModelFactory, m_RenderSchemeFactorySet)
    {
        currentModelFactory->Setup();
    }

    // Setup is the first time update() therefore update ALL
    m_DirtyType = SCENE_DIRTY_TYPE::ALL;
}

void Scene::Update()
{
    foreach(RenderSchemeFactory* currentModelFactory, m_RenderSchemeFactorySet)
    {
        currentModelFactory->UpdateUniform();
    }
    if (!IsDirty()) return;

    foreach(RenderSchemeFactory* currentModelFactory, m_RenderSchemeFactorySet)
    {
        glm::mat4 transformation = *m_Transform.GetProjectionMatrix() * *m_Transform.GetViewMatrix();
        currentModelFactory->SetRefProjectViewMatrix(transformation);
    }

    const SCENE_DIRTY_TYPE updateTransformType = static_cast<SCENE_DIRTY_TYPE>(static_cast<int>(m_DirtyType) & static_cast<int>(SCENE_DIRTY_TYPE::TRANSFORMATION));
    if (updateTransformType == SCENE_DIRTY_TYPE::TRANSFORMATION)
    {
        foreach (Node* item, m_RootDrawableList)
        {
            assert(item);

            item->Update();
        }
    }

//    SCENE_DIRTY_TYPE updateItemType = static_cast<SCENE_DIRTY_TYPE>(static_cast<int>(m_DirtyType) & static_cast<int>(SCENE_DIRTY_TYPE::ALL_ITEMS));
//    if (updateItemType == SCENE_DIRTY_TYPE::ALL_ITEMS)
//    {
//        foreach(RenderSchemeFactory* currentModelFactory, m_RenderSchemeFactorySet)
//        {
//            currentModelFactory->Update();
//        }
//    }
//    else
//    {
//        updateItemType = static_cast<SCENE_DIRTY_TYPE>(static_cast<int>(m_DirtyType) & static_cast<int>(SCENE_DIRTY_TYPE::DIRTY_ITEMS));
//        if (updateItemType == SCENE_DIRTY_TYPE::DIRTY_ITEMS)
//        {
//            foreach(RenderSchemeFactory* currentModelFactory, m_RenderSchemeFactorySet)
//            {
//                currentModelFactory->UpdateDirty();
//            }
//        }
//    }


    foreach (RenderSchemeFactory* currentModelFactory, m_RenderSchemeFactorySet)
    {
        if (!currentModelFactory) continue;

        if (static_cast<int>(m_DirtyType) & static_cast<int>(SCENE_DIRTY_TYPE::TRANSFORMATION))
        {
            currentModelFactory->Update();
        }

#if DIRTY_UPDATE_IMPLEMENTED == 1
        else if (static_cast<int>(m_DirtyType) & static_cast<int>(SCENE_DIRTY_TYPE::DIRTY_ITEMS))
        {
            currentModelFactory->UpdateDirty();
        }
#endif
    }

    m_DirtyType = SCENE_DIRTY_TYPE::NONE;
}

void Scene::Render(VkCommandBuffer& p_CommandBuffer)
{
    foreach(RenderSchemeFactory* currentModelFactory, m_RenderSchemeFactorySet)
    {
        currentModelFactory->Render(p_CommandBuffer);
    }
}

// The early depth testing
/*
1. Draw two rectangles(with partial alpha) in order RED(Depth 0) > GREEN(Depth 0)
Expect output: Draw Red(Below) and Green(Above)
Actual output: Draw Red(Below) and Green(Above)

2. Draw two rectangles(with partial alpha) in order RED(Depth 15) > GREEN(Depth 0)
Expect output: Draw Red(Above) and Green(Below)
Actual output: Draw Red(Above) and Green(Below), the alpha of Red above create artefact on the green rect.
               (The alpha region of red eats aways the overlap portion of Green Rectangle)

Fix: SetEarlyDepthTest(true);
Expect output: Draw Red(Above) and Green(Below)
Actual output: Draw Red(Above) and Green(Below), the alpha of Red rectangle appears correct on of top Green one.
*/
void Scene::GatherFlatNodeList()
{
    m_FlatList.clear();

    foreach (Node* item, m_RootDrawableList)
    {
        assert(item);

        item->GatherFlatNodeList();
    }

    if (m_EarlyDepthTest)
    {
        // Sort Z-Order
        std::sort(m_FlatList.begin(), m_FlatList.end(),
            [](const Node* p_Item1, const Node* p_Item2) -> bool
        {
            if (!p_Item1 || !p_Item2) return true;

            return (p_Item1->GetBoundedRegion().m_Position.z < p_Item2->GetBoundedRegion().m_Position.z);
        });
    }
}

void Scene::AddItem(Node* p_Item)
{
    if (p_Item && !p_Item->GetParent())
    {
        m_RootDrawableList.push_back(p_Item);
    }
}


// While removing the model remove it from model list and flat list.
void Scene::RemoveItem(Node* p_Item)
{
    while (true)
    {
        auto result = std::find(std::begin(m_RootDrawableList), std::end(m_RootDrawableList), p_Item);
        if (result == std::end(m_RootDrawableList)) break;

        m_RootDrawableList.erase(result);
    }

    while (true)
    {
        auto result = std::find(std::begin(m_FlatList), std::end(m_FlatList), p_Item);
        if (result == std::end(m_FlatList)) break;

        m_FlatList.erase(result);
    }

    RenderSchemeFactory* factory = GetRenderSchemeFactory(p_Item); // Populate factories
    if (!factory) return;

    factory->RemoveNodeList(p_Item);
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
//    foreach (Node* item, m_RootDrawableList)
//    {
//        assert(item);

//        item->mousePressEvent(p_Event);
//    }

    static Node* oldModelItem = NULL;
    for (int i = m_RootDrawableList.size() - 1; i >= 0; i--)
    {
        Node* item = m_RootDrawableList.at(i);
        assert(item);

        item->mousePressEvent(p_Event);
        if (p_Event->isAccepted())
        {
            Node* currentModel = GetCurrentHoverItem();
            if (oldModelItem && oldModelItem != currentModel)
            {
                oldModelItem->SetColor(oldModelItem->GetDefaultColor());
            }

            currentModel->SetColor(glm::vec4(1.0, 1.0, 0.3, 0.5));
            oldModelItem = GetCurrentHoverItem();

            QMessageBox msgBox;
            msgBox.setText(QString("Item: %1 Clicked").arg(currentModel->GetName()));
            msgBox.exec();

            return;
        }
    }

    if (oldModelItem)
    {
        oldModelItem->SetColor(oldModelItem->GetDefaultColor());
    }
}

void Scene::mouseReleaseEvent(QMouseEvent* p_Event)
{
    foreach (Node* item, m_RootDrawableList)
    {
        assert(item);

        item->mouseReleaseEvent(p_Event);
    }
}

void Scene::mouseMoveEvent(QMouseEvent* p_Event)
{
    static Node* oldModelItem = NULL;
    for (int i = m_RootDrawableList.size() - 1; i >= 0; i--)
    {
        Node* item = m_RootDrawableList.at(i);
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
    if ((shapeType <= SHAPE::SHAPE_NONE) && (shapeType >= SHAPE::SHAPE_COUNT)) return NULL;

    typedef std::multimap<SHAPE, RenderSchemeFactory*>::iterator MMAPIterator;
    std::pair<MMAPIterator, MMAPIterator> result = m_ShapeRenderSchemeTypeMap.equal_range(shapeType);
    for (MMAPIterator it = result.first; it != result.second; it++)
    {
        if (it->second->GetMemPoolIdx() == p_Item->GetMemPoolIdx())
            return it->second;
    }

//    std::map<SHAPE, RenderSchemeFactory*>::iterator it = m_ShapeRenderSchemeTypeMap.find(shapeType);
//    if (it != m_ShapeRenderSchemeTypeMap.end())
//    {
//        return it->second;
//    }

    RenderSchemeFactory* renderSchemeFactoryItem = p_Item->GetRenderSchemeFactory();
    if (!renderSchemeFactoryItem) return NULL;

    renderSchemeFactoryItem->SetMemPoolIdx(p_Item->GetMemPoolIdx());
    m_ShapeRenderSchemeTypeMap.insert (std::make_pair(shapeType, renderSchemeFactoryItem));

    return renderSchemeFactoryItem;
}
/*
RenderSchemeFactory* Scene::GetRenderSchemeFactory(Node* p_Item)
{
    const SHAPE shapeType = p_Item->GetShapeType();
    if ((shapeType <= SHAPE::SHAPE_NONE) && (shapeType >= SHAPE::SHAPE_COUNT)) return NULL;

    std::map<SHAPE, RenderSchemeFactory*>::iterator it = m_ShapeRenderSchemeTypeMap.find(shapeType);
    if (it != m_ShapeRenderSchemeTypeMap.end())
    {
        return it->second;
    }

    RenderSchemeFactory* renderSchemeFactoryItem = p_Item->GetRenderSchemeFactory();
    if (renderSchemeFactoryItem)
    {
        (m_ShapeRenderSchemeTypeMap)[shapeType] = renderSchemeFactoryItem;
    }

    return renderSchemeFactoryItem;
}
*/
void Scene::AppendToFlatNodeList(Node* p_Item)
{
    m_FlatList.push_back(p_Item);
}
