#include "InstancingDemoApp.h"
#include "../../../common/VulkanHelper.h"
#include "Rect.h"

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

//    m_CubeFactory = RectangleMultiDrawFactory::SingleTon(this);

    m_Scene = new Scene3D(this);

	//m_Cube = new RectangleModel(this, m_Scene, NULL, "Rectangle 1", SHAPE_RECTANGLE, RENDER_SCEHEME_MULTIDRAW);
 //   m_Cube->Rectangle(100, 100 , 100, 100);
 //   m_Cube->SetColor(glm::vec4(0.6, 0.2, 0.20, 1.0));
 //   m_Cube->SetDefaultColor(glm::vec4(0.42, 0.15, 0.60, 1.0));

 //   m_Cube = new RectangleModel(this, m_Scene, m_Cube, "Rectangle 2", SHAPE_RECTANGLE, RENDER_SCEHEME_MULTIDRAW);
 //   m_Cube->Rectangle(100, 100, 50, 50);
 //   m_Cube->SetColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
 //   m_Cube->SetDefaultColor(glm::vec4(0.42, 0.15, 0.60, 1.0));

//	Grid(m_Scene);
    MixerView(m_Scene);
}

InstancingDemoApp::~InstancingDemoApp()
{
    delete m_Scene;
}

void InstancingDemoApp::Grid(Scene3D* m_Scene)
{
	float parentCol = 2;
	float parentRow = 2;
	float parentColWidth = m_windowDim.width / parentCol;
	float parentColHeight = m_windowDim.height / parentRow;

	const float Col = 2;
	const float Row = 1;
	float colWidth = parentColWidth / Col;
	float colHeight = parentColHeight / Row;

	for (int i = 0; i < parentCol; i++)
	{
		for (int j = 0; j < parentRow; j++)
		{
			Model3D* m_Parent = new RectangleModel(NULL, m_Scene, NULL, "Node 1", SHAPE_RECTANGLE, RENDER_SCEHEME_MULTIDRAW);
			m_Parent->Rectangle((i * parentColWidth), (j * parentColHeight), parentColWidth - 2, parentColHeight);
			m_Parent->SetColor(glm::vec4(0.6, 0.2, 0.20, 1.0));
			m_Parent->SetDefaultColor(glm::vec4(0.42, 0.15, 0.60, 1.0));

			for (int k = 0; k < Col; k++)
			{
				for (int l = 0; l < Row; l++)
				{
					RectangleModel* m_Cube1 = new RectangleModel(NULL, m_Scene, m_Parent, "Node 1", SHAPE_RECTANGLE, RENDER_SCEHEME_MULTIDRAW);
					m_Cube1->Rectangle((k * colWidth), (l * colHeight), colWidth, colHeight);
					m_Cube1->SetColor(glm::vec4(0.2, 0.5, 0.50, 1.0));
					m_Cube1->SetDefaultColor(glm::vec4(0.2, 0.5, 0.50, 1.0));
				}
			}
		}
	}
}

void InstancingDemoApp::MixerView(Scene3D* m_Scene)
{
    const float mixerPanelWidth = m_windowDim.width;
    const float mixerPanelHeight = m_windowDim.height;

    const float mixerWidth = 100;
    const int numberOfMixers = mixerPanelWidth / mixerWidth;

    for (int i = 0; i < numberOfMixers; i++)
    {
        glm::vec2 p_TopLeftPos((i * mixerWidth), 0);
        glm::vec2 p_Dim(mixerWidth, mixerPanelHeight);
        AudioMixerItem* m_MixerItem = new AudioMixerItem(m_Scene, NULL, "Mixer Item 1", p_TopLeftPos, p_Dim, SHAPE_CUSTOM);
        m_MixerItem->SetColor(glm::vec4(1.1, 0.2, 0.20, 1.0));
        m_MixerItem->SetDefaultColor(glm::vec4(1.0, 0.15, 0.60, 1.0));
    }
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

	//static glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	//m_Scene->SetProjection(&Projection);

	//static glm::mat4 View = glm::lookAt(glm::vec3(100, -200, 300), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	//m_Scene->SetView(&View);
    static glm::mat4 Projection = glm::ortho(0.0f, static_cast<float>(m_windowDim.width), 0.0f, static_cast<float>(m_windowDim.height));
    m_Scene->SetProjection(&Projection);

    //static glm::mat4 View = glm::lookAt(glm::vec3(0, 0, 1500), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    //View = glm::translate(View, glm::vec3(-1000, -500, 0));
    static glm::mat4 View;
    m_Scene->SetView(&View);

    m_Scene->SetUpProjection(); // For some reason the ViewMatrix is not working properly, this setupensure model matrix is set properly.

	m_Scene->Setup(); // Create the object's vertex buffer
}

void InstancingDemoApp::Update()
{
    static float phi = 0.0f;
    //glm::mat4 View = glm::lookAt(glm::vec3(500, 500, 500) * sin(phi += 0.01), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    //m_Scene->SetView(&View);
    static glm::mat4 View;
    m_Scene->SetView(&View);

	// TODO: Add dirty flag check to avoid constant update

	static int i = 0;
	i++;
	// This is a test check to see the perform
	if (i < 20)
	{
		printf("\n Update: %d......", i);
		m_Scene->Update();
	}
}

bool InstancingDemoApp::Render()
{
    // Important: Uncomment below line only if there are updates for model expected in the Model
    // 1. It has been observed that the re-recording of command buffer in case of non-instance drawing is expensive
    
    // m_Scene->Render(); //Read the note before uncommenting

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
