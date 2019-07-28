#include "MeshDemoApp.h"
#include "../../../common/VulkanHelper.h"

#include "SimpleMesh.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QHBoxLayout>
#include <QApplication>

MeshDemoApp::MeshDemoApp()
{
	VulkanHelper::GetInstanceLayerExtensionProperties();
	m_Cube = new SimpleMesh(this);
}

MeshDemoApp::~MeshDemoApp()
{
	delete m_Cube;
}

void MeshDemoApp::Configure()
{
    SetApplicationName("Light shading - Diffuse");
    SetWindowDimension(800, 600);

    // Add Validation Layers
    AddValidationLayer("VK_LAYER_LUNARG_standard_validation");

    // Add Vulkan instance extensions
    AddInstanceExtension(VK_KHR_SURFACE_EXTENSION_NAME);
    AddInstanceExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    AddInstanceExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

	static glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	static glm::mat4 View = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	m_Cube->SetProjection(&Projection);
	m_Cube->SetView(&View);
}

void MeshDemoApp::Setup()
{
	m_Cube->LoadMesh("../../../resources/models/suzanne.obj");

	static glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	m_Cube->SetProjection(&Projection);

	static glm::mat4 View = glm::lookAt(glm::vec3(0, -10, 35), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	m_Cube->SetView(&View);

	m_Cube->Setup();
}

void MeshDemoApp::Update()
{
	m_Cube->Update();
}

void MeshDemoApp::ResizeWindow(int width, int height)
{
    VulkanApp::ResizeWindow(width, height);

	float aspectRatio = float(width) / float(height);
	Projection = glm::perspective(glm::radians(45.0f), (width > height) ? aspectRatio : (1.0f / aspectRatio), 0.1f, 100.0f);
	m_Cube->SetProjection(&Projection);

    m_Cube->ResizeWindow(width, height);
}

int main(int argc, char **argv)
{
	QApplication qtApp(argc, argv);

	MainWindow mainWindow;
	QWidget* widget[4] = { mainWindow.ui->widget, mainWindow.ui->widget_2, mainWindow.ui->widget_3, mainWindow.ui->widget_4 };
	for (int i = 0; i < 4; ++i)
	{
		MeshDemoApp* meshApp = new MeshDemoApp(); // Create Vulkan app instance
		meshApp->EnableDepthBuffer(true);
		meshApp->EnableWindowResize(true);
		meshApp->Initialize();

		widget[i]->layout()->addWidget(QWidget::createWindowContainer(meshApp->m_pWindow));
	}
	mainWindow.show();

	qtApp.exec();

	//delete meshApp;
	return 0;
}