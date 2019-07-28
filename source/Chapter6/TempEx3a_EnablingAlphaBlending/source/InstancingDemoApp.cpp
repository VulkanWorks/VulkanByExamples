#include "InstancingDemoApp.h"
#include "UIDemo.h"

#include <QApplication>

int main(int argc, char **argv)
{
    QApplication qtApp(argc, argv);

    InstancingDemoApp* instanceDemo = new InstancingDemoApp();
    instanceDemo->SetWindowDimension(800, 600);
    instanceDemo->EnableDepthBuffer(true);
    instanceDemo->EnableWindowResize(true);
    instanceDemo->Initialize();
    qtApp.exec();

    delete instanceDemo;
    return 0;
}

InstancingDemoApp::InstancingDemoApp()
{
    VulkanHelper::GetInstanceLayerExtensionProperties();

    m_Scene = new Scene3D(this);

    m_Cube1 = new RectangleModel(m_Scene, NULL, BoundingRegion(200, 200, 100, 100));
    m_Cube1->SetColor(glm::vec4(0.6, 0.2, 0.20, 1.0));
    m_Cube1->SetDefaultColor(glm::vec4(0.42, 0.15, 0.60, 1.0));

    m_Cube2 = new RectangleModel(m_Scene, m_Cube1, BoundingRegion(100, 100, 50, 50));
    m_Cube2->SetColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
    m_Cube2->SetDefaultColor(glm::vec4(0.42, 0.15, 0.60, 1.0));

    m_Cube3 = new RectangleModel(m_Scene, m_Cube1, BoundingRegion(0, 0, 50, 50));
    m_Cube3->SetColor(glm::vec4(0.6, 0.0, 1.0, 1.0));
    m_Cube3->SetDefaultColor(glm::vec4(0.2, 0.55, 0.20, 1.0));

    m_Cube4 = new RectangleModel(m_Scene, m_Cube1, BoundingRegion(100, 0, 50, 50));
    m_Cube4->SetColor(glm::vec4(0.0, 0.2, 1.0, 1.0));
    m_Cube4->SetDefaultColor(glm::vec4(0.2, 0.35, 0.30, 1.0));

    m_Cube5 = new RectangleModel(m_Scene, m_Cube1, BoundingRegion(0, 100, 50, 50));
    m_Cube5->SetColor(glm::vec4(0.0, 0.5, 1.0, 1.0));
    m_Cube5->SetDefaultColor(glm::vec4(0.62, 0.25, 0.60, 1.0));

//    m_UIDemo.Grid(m_Scene, m_windowDim.width, m_windowDim.height);
//    m_UIDemo.MixerView(m_Scene, m_windowDim.width, m_windowDim.height);
}

InstancingDemoApp::~InstancingDemoApp()
{
    delete m_Scene;
}

void InstancingDemoApp::Configure()
{
    SetApplicationName("Instancing Demo");
    SetWindowDimension(800, 600);

    // Add Validation Layers
    AddValidationLayer("VK_LAYER_LUNARG_standard_validation");

    // Add Vulkan instance extensions
    AddInstanceExtension(VK_KHR_SURFACE_EXTENSION_NAME);
    AddInstanceExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    AddInstanceExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
}

void InstancingDemoApp::Setup()
{
    // Note: We are overidding the default Create Command pool with VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
    // because we need to re-record the command buffer when the instance data size changes. 
    // This need to recreate the command buffer. 
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = m_physicalDeviceInfo.graphicsFamilyIndex;
    VulkanHelper::CreateCommandPool(m_hDevice, m_hCommandPool, m_physicalDeviceInfo, &poolInfo);

    m_Scene->SetUpProjection();

	m_Scene->Setup();
}

void InstancingDemoApp::Update()
{
    static float rot = 0.0;
    if (m_Cube1)
        m_Cube1->Rotate(rot = .001, 0.0, 0.0, 1.0);
    
    if (m_Cube2)
    {
        m_Cube2->Rotate(rot = .003, 0.0, 0.0, 1.0);
        m_Cube3->Rotate(rot = .003, 0.0, 0.0, 1.0);
        m_Cube4->Rotate(rot = .003, 0.0, 0.0, 1.0);
        m_Cube5->Rotate(rot = .003, 0.0, 0.0, 1.0);
    }
    // Note: There are two ways to apply update
    // 1. Scene Update: This will traverse all childs and apply updates (like creating vertex buffer) depending upon the derivation implementation.
    m_Scene->Update();

    // 2. Model Update: This update will not bother the all model nodes to update but only the intended one with its children.
    //m_Cube1->UpdateMeAndMyChildren();

    // Note: 
}

bool InstancingDemoApp::Render()
{
    // Important: Uncomment below line only if there are updates for model expected in the Model
    // 1. It has been observed that the re-recording of command buffer in case of non-instance drawing is expensive
    
    m_Scene->Render(); //Read the note before uncommenting

    return VulkanApp::Render();
}


void InstancingDemoApp::mousePressEvent(QMouseEvent* p_Event)
{
    m_Scene->mousePressEvent(p_Event);
}

void InstancingDemoApp::mouseReleaseEvent(QMouseEvent* p_Event)
{
    m_Scene->mouseReleaseEvent(p_Event);
}

void InstancingDemoApp::mouseMoveEvent(QMouseEvent* p_Event)
{
    m_Scene->mouseMoveEvent(p_Event);
}

void InstancingDemoApp::ResizeWindow(int width, int height)
{
    VulkanApp::ResizeWindow(width, height);

	m_Scene->Resize(width, height);
}
