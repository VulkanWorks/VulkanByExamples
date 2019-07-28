#include "InstancingDemoApp.h"
#include "../../../common/VulkanHelper.h"
#include "Cube.h"

#include <QMainWindow>
#include <QHBoxLayout>
#include <QApplication>

int main(int argc, char **argv)
{
	QApplication qtApp(argc, argv);

    InstancingDemoApp* instanceDemo = new InstancingDemoApp(); // Create Vulkan app instance
    instanceDemo->SetWindowDimension(800, 600);    // Default application window dimension
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
	m_Cube = new Cube(this);
}

InstancingDemoApp::~InstancingDemoApp()
{
	delete m_Cube;
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
	static glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	m_Cube->SetProjection(&Projection);

	static glm::mat4 View = glm::lookAt(glm::vec3(10, -20, 30), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	m_Cube->SetView(&View);

	m_Cube->Setup();
}

void InstancingDemoApp::Update()
{
	static float rot = 0;
	m_Cube->Rotate(rot += .001f, 0.0f, 1.0f, 0.0f);
	m_Cube->Update();
}

void InstancingDemoApp::ResizeWindow(int width, int height)
{
    VulkanApp::ResizeWindow(width, height);

	m_Cube->ResizeWindow(width, height);
}
