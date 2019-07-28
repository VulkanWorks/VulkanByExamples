#include "UIDemoApp.h"
#include "Rect.h"

#include <QApplication>

int main(int argc, char **argv)
{
    QApplication qtApp(argc, argv);

    UIDemoApp* instanceDemo = new UIDemoApp();
    instanceDemo->EnableDepthBuffer(true);
    instanceDemo->EnableWindowResize(true);
    instanceDemo->Initialize();
    qtApp.exec();

    delete instanceDemo;
    return 0;
}

UIDemoApp::UIDemoApp()
{
    VulkanHelper::GetInstanceLayerExtensionProperties();

    m_SceneVector.push_back(std::make_shared<Scene>(this));
    m_Scene = m_SceneVector[0].get();

    m_RectTr1 = new Rectangl(m_Scene, NULL, BoundingRegion(200, 200, 100, 100));
    m_RectTr1->SetColor(glm::vec4(0.6, 0.2, 0.20, 1.0));
    m_RectTr1->SetZOrder(1.1);

    m_RectTr2 = new Rectangl(m_Scene, m_RectTr1, BoundingRegion(100, 100, 50, 50));
    m_RectTr2->SetColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
   
    m_RectTr3 = new Rectangl(m_Scene, m_RectTr1, BoundingRegion(0, 0, 50, 50));
    m_RectTr3->SetColor(glm::vec4(0.6, 0.0, 1.0, 1.0));

    m_RectTr4 = new Rectangl(m_Scene, m_RectTr1, BoundingRegion(75, -25, 50, 50));
    m_RectTr4->SetZOrder(-10.1);
    m_RectTr4->SetOriginOffset(glm::vec3(25, 25, 0));
    m_RectTr4->SetColor(glm::vec4(0.0, 0.2, 1.0, 1.0));

    {
        float x = 0;
        float y = 0;
        m_Rect1 = new Rectangl(m_Scene, NULL, BoundingRegion(x, y, 100, 100, -1), "Item1");
        m_Rect1->SetColor(glm::vec4(0.6, 0.2, 0.20, 1.0));

        m_Rect2 = new Rectangl(m_Scene, m_Rect1, BoundingRegion(x += 50, y, 100, 100, -1), "Item2");
        m_Rect2->SetColor(glm::vec4(1.0, 0.2, 0.20, 1.0));

        m_Rect3 = new Rectangl(m_Scene, m_Rect1, BoundingRegion(x += 50, y, 100, 100, 10), "Item3");
        m_Rect3->SetColor(glm::vec4(1.0, 1.2, 0.20, 1.0));

        m_Rect4 = new Rectangl(m_Scene, m_Rect1, BoundingRegion(x += 50, y, 100, 100, -1000), "Item4");
        m_Rect4->SetColor(glm::vec4(1.0, 1.2, 1.0, 1.0));
    }
}

UIDemoApp::~UIDemoApp()
{
    delete m_Scene;
}

void UIDemoApp::Configure()
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

void UIDemoApp::Setup()
{
    m_Scene->SetUpProjection();
    m_Scene->Setup();

    RecordRenderPass(1, SG_STATE_SETUP);

    // At least update the scene once so that in case UpdateMeAndMyChildren() is being used it has all transformation readily available
    m_Scene->Update();
}

void UIDemoApp::Update()
{
    static float rot = 0.0;
    if (m_RectTr1)
    {
        m_RectTr1->Rotate(.001, 0.0, 0.0, 1.0);
     
        m_RectTr2->Reset();
        m_RectTr2->SetPosition(100, 100);
        m_RectTr2->Rotate(rot += .1, 0.0, 0.0, 1.0);
        
        m_RectTr3->Rotate(.003, 0.0, 0.0, 1.0);
        m_RectTr4->Rotate(.003, 0.0, 0.0, 1.0);
    }
    {
        m_Rect1->Rotate(.003, 0.0, 0.0, 1.0);
        m_Rect2->Rotate(.003, 0.0, 0.0, 1.0);
        m_Rect3->Rotate(.003, 0.0, 0.0, 1.0);
        m_Rect4->Rotate(.003, 0.0, 0.0, 1.0);
    }

    // Note: There are two ways to apply update
    // 1. Scene Update: This will traverse all childs and apply updates (like creating vertex buffer) depending upon the derivation implementation.
    m_Scene->Update();

    // 2. Model Update: This update will not bother the all model nodes to update but only the intended one with its children.
    //m_Cube2->UpdateMeAndMyChildren();
}

bool UIDemoApp::Render()
{
    RecordRenderPass(1, SG_STATE_RENDER);

    return VulkanApp::Render();
}

void UIDemoApp::ResizeWindow(int p_Width, int p_Height)
{
    VulkanApp::ResizeWindow(p_Width, p_Height);

    RecordRenderPass(3, SG_STATE_RESIZE, p_Width, p_Height);
}

void UIDemoApp::RecordRenderPass(int p_Argcount, ...)
{
    va_list list;
    va_start(list, p_Argcount);
    SCENE_GRAPH_STATES currentState = SG_STATE_NONE;
    QSize resizedDimension;

    for (int i = 0; i < p_Argcount; ++i)
    {
        switch (i)
        {
        case 0:
            currentState = static_cast<SCENE_GRAPH_STATES>(va_arg(list, int));
            break;

        case 1:
            resizedDimension.setWidth(va_arg(list, int));
            break;

        case 2:
            resizedDimension.setHeight(va_arg(list, int));
            break;

        default:
            if (currentState == SG_STATE_NONE)
                return;
            break;
        }
    }
    va_end(list);

    // Specify the clear color value
    VkClearValue clearColor[2];
    clearColor[0].color.float32[0] = 0.0f;
    clearColor[0].color.float32[1] = 0.0f;
    clearColor[0].color.float32[2] = 0.0f;
    clearColor[0].color.float32[3] = 0.0f;

    // Specify the depth/stencil clear value
    clearColor[1].depthStencil.depth = 1.0f;
    clearColor[1].depthStencil.stencil = 0;

    // Offset to render in the frame buffer
    VkOffset2D   renderOffset = { 0, 0 };
    // Width & Height to render in the frame buffer
    VkExtent2D   renderExtent = m_swapChainExtent;

    // For each command buffers in the command buffer list
    for (size_t i = 0; i < m_hCommandBufferList.size(); i++)
    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        // Indicate that the command buffer can be resubmitted to the queue
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        // Begin command buffer
        vkBeginCommandBuffer(m_hCommandBufferList[i], &beginInfo);

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_hRenderPass;
        renderPassInfo.framebuffer = m_hFramebuffers[i];
        renderPassInfo.renderArea.offset = renderOffset;
        renderPassInfo.renderArea.extent = renderExtent;
        renderPassInfo.clearValueCount = 2;
        renderPassInfo.pClearValues = clearColor;

        // Begin render pass
        vkCmdBeginRenderPass(m_hCommandBufferList[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        switch (currentState)
        {
        case SG_STATE_SETUP:
            m_Scene->SetupRenderFactory(m_hCommandBufferList[i]);
            break;

        case SG_STATE_RENDER:
            m_Scene->Render(m_hCommandBufferList[i]);
            break;

        case SG_STATE_RESIZE:
            m_Scene->Resize(m_hCommandBufferList[i], resizedDimension.width(), resizedDimension.height());
            break;

        default:
            break;
        }

        // End the Render pass
        vkCmdEndRenderPass(m_hCommandBufferList[i]);

        // End the Command buffer
        VkResult vkResult = vkEndCommandBuffer(m_hCommandBufferList[i]);
        if (vkResult != VK_SUCCESS)
        {
            VulkanHelper::LogError("vkEndCommandBuffer() failed!");
            assert(false);
        }
    }
}
