#include "MyFirst3DApp.h"
#include "../../../common/VulkanHelper.h"
#include "Cube.h"

#include <QMainWindow>
#include <QHBoxLayout>
#include <QApplication>

int main(int argc, char **argv)
{
	QApplication qtApp(argc, argv);

	MyFirst3DApp* helloVulkanApp = new MyFirst3DApp(); // Create Vulkan app instance
	helloVulkanApp->SetWindowDimension(800, 600);    // Default application window dimension
	helloVulkanApp->EnableDepthBuffer(true);
	helloVulkanApp->Initialize();
	qtApp.exec();

	delete helloVulkanApp;
	return 0;
}

MyFirst3DApp::MyFirst3DApp()
{
	VulkanHelper::GetInstanceLayerExtensionProperties();
    m_CubeFactory = CubeFactory::SingleTon(this);

	m_Scene = new Scene3D();
    
//	m_Cube1 = new Model3D(m_Scene, NULL, "Node 1", SHAPE_CUBE);
	Model3D* lastModel = NULL;
	for (int i = 0; i < 10; i++)
	{
		lastModel = new Model3D(m_Scene, lastModel, "Node 2", SHAPE_CUBE);
		if (i == 0) m_Cube1 = lastModel;

		lastModel->Translate(0.0f, 0.0f, 3.0f);
	}
}

MyFirst3DApp::~MyFirst3DApp()
{
}

void MyFirst3DApp::Configure()
{
    SetApplicationName("My First 3D Application - Depth buffer");
    SetWindowDimension(800, 600);

    // Add Validation Layers
    AddValidationLayer("VK_LAYER_LUNARG_standard_validation");

    // Add Vulkan instance extensions
    AddInstanceExtension(VK_KHR_SURFACE_EXTENSION_NAME);
    AddInstanceExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    AddInstanceExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
}

void MyFirst3DApp::Setup()
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
	m_CubeFactory->Setup();
}

void MyFirst3DApp::Update()
{
	static float rot = 0;
	m_Cube1->Rotate(rot = .01f, 0.0f, 1.0f, 0.0f);
	
//	Sleep(1000);
	m_Scene->SetUpProjection();
	m_Scene->Update();
	m_CubeFactory->prepareInstanceData(m_Scene);
}