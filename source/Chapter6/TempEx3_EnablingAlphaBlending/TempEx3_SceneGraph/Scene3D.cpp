#include "Scene3D.h"
#include "Model3D.h"

#include "../../common/VulkanApp.h" // Not a good design to include vulkan app here: Todo move AbstractApp 
#include <QMouseEvent> 
/*extern*/ bool isDirty;

Scene3D::Scene3D(AbstractApp* p_Application)
    : m_Application(p_Application)
    , m_Frame(0)
    , m_ScreenWidth(800)
    , m_ScreenHeight(600)
    , m_CurrentHoverItem(NULL)
    , m_Projection(NULL)	// Not owned by Scene, double check this can be owned. TODO: PS
    , m_View(NULL)		// Not owned by Scene
{
}

Scene3D::~Scene3D()
{
    RenderSchemeTypeMap* m_FactoryMap = NULL;
    std::map<SHAPE, RenderSchemeTypeMap*>::iterator itSRST = m_ShapeRenderSchemeTypeMap.begin();

    while (itSRST != m_ShapeRenderSchemeTypeMap.end())
    {
        m_FactoryMap = itSRST->second;
        std::map<RENDER_SCEHEME_TYPE, AbstractModelFactory*>::iterator it = m_FactoryMap->begin();
        if (it != m_FactoryMap->end())
        {
            delete it->second;
        }

        ++itSRST;
    }

    foreach (Model3D* currentModel, m_ModelList)
    {
        delete currentModel;
    }
}

void Scene3D::Setup()
{
    GatherFlatList(); // Assuming all nodes are added into the scenes by now

    foreach (Model3D* currentModel, m_ModelList)
    {
        currentModel->Setup();
    }

    foreach(Model3D* currentModel, m_FlatList)
    {
        AbstractModelFactory* factory = GetFactory(currentModel); // Populate factories
        if (!factory) continue;

        factory->UpdateModelList(currentModel);
    }

    ////////////////////////////////////////////////
    RenderSchemeTypeMap* m_FactoryMap = NULL;
    std::map<SHAPE, RenderSchemeTypeMap*>::iterator itSRST = m_ShapeRenderSchemeTypeMap.begin();
    if (itSRST != m_ShapeRenderSchemeTypeMap.end())
    {
        m_FactoryMap = itSRST->second;
        std::map<RENDER_SCEHEME_TYPE, AbstractModelFactory*>::iterator it = m_FactoryMap->begin();
        if (it != m_FactoryMap->end())
        {
            m_ModelFactories.insert(it->second);
            it++;
        }

        itSRST++;
    }

    ///////////////////////////////////////////////
    foreach(Model3D* currentModel, m_FlatList)
    {
//        m_ModelFactories.insert(currentModel->m_AbstractFactory);
    }

	assert(m_ModelFactories.size());
	foreach(AbstractModelFactory* currentModelFactory, m_ModelFactories)
    {
        currentModelFactory->Setup();
    }
}

void Scene3D::Update()
{
    foreach(AbstractModelFactory* currentModelFactory, m_ModelFactories)
    {
        currentModelFactory->m_Transform = (*GetProjection()) * (*GetView());
    }

    foreach (Model3D* item, m_ModelList)
    {
        assert(item);

        item->Update();
    }

	foreach(AbstractModelFactory* currentModelFactory, m_ModelFactories)
	{
		currentModelFactory->Update();
	}

//	m_CubeFactory->prepareInstanceData(m_Scene);
}

void Scene3D::Render()
{
    foreach(AbstractModelFactory* currentModelFactory, m_ModelFactories)
    {
        currentModelFactory->Render();
    }
}

void Scene3D::GatherFlatList()
{
    m_FlatList.clear();

    foreach (Model3D* item, m_ModelList)
    {
        assert(item);

        item->GatherFlatList();
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

	foreach(AbstractModelFactory* currentModelFactory, m_ModelFactories)
	{
		currentModelFactory->ResizeWindow(p_Width, p_Height);
	}
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

AbstractModelFactory* Scene3D::GetFactory(Model3D* p_Model)
{
    const SHAPE shapeType = p_Model->GetShapeType();
    if ((shapeType <= SHAPE_NONE) && (shapeType >= SHAPE_COUNT)) return NULL;

    RenderSchemeTypeMap* m_FactoryMap = NULL;
    std::map<SHAPE, RenderSchemeTypeMap*>::iterator itSRST = m_ShapeRenderSchemeTypeMap.find(shapeType);
    if (itSRST != m_ShapeRenderSchemeTypeMap.end())
    {
        m_FactoryMap = itSRST->second;
    }
    else
    {
        m_FactoryMap = new RenderSchemeTypeMap();
        m_ShapeRenderSchemeTypeMap[shapeType] = m_FactoryMap;
    }

    const RENDER_SCEHEME_TYPE renderSchemeType = p_Model->GetRenderSchemeType();
    std::map<RENDER_SCEHEME_TYPE, AbstractModelFactory*>::iterator it = m_FactoryMap->find(renderSchemeType);
    if (it != m_FactoryMap->end())
    {
        return it->second;
    }

    AbstractModelFactory* abstractFactory = p_Model->GetRenderScemeFactory();
    if (abstractFactory)
    {
        (*m_FactoryMap)[renderSchemeType] = abstractFactory;
    }

    return abstractFactory;
}

