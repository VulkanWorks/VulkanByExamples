#include "InstancingDemoApp.h"
#include "../../../common/VulkanHelper.h"
#include "Rect.h"

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

	m_Cube = new RectangleModel(m_Scene, NULL, BoundingRegion(100, 100, 100, 100));
    m_Cube = new RectangleModel(m_Scene, m_Cube, BoundingRegion(100, 100, 50, 50));
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
    m_Cube->Rotate(rot++, 0.0, 0.0, 1.0);
 	m_Scene->Update();
}

bool InstancingDemoApp::Render()
{
    m_Scene->Render();

    return VulkanApp::Render();
}

void InstancingDemoApp::ResizeWindow(int width, int height)
{
    VulkanApp::ResizeWindow(width, height);

	m_Scene->Resize(width, height);
}
