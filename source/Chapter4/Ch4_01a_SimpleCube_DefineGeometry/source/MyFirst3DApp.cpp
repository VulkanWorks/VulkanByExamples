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
	helloVulkanApp->EnableDepthBuffer(false);
	helloVulkanApp->Initialize();
	qtApp.exec();

	delete helloVulkanApp;
	return 0;
}

MyFirst3DApp::MyFirst3DApp()
{
	VulkanHelper::GetInstanceLayerExtensionProperties();
	m_Cube = new Cube(this);
}

MyFirst3DApp::~MyFirst3DApp()
{
	delete m_Cube;
}

void MyFirst3DApp::Configure()
{
    SetApplicationName("My First 3D Application - Defining Geometry");
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
	m_Cube->Setup();
}

void MyFirst3DApp::Update()
{
	m_Cube->Update();
}