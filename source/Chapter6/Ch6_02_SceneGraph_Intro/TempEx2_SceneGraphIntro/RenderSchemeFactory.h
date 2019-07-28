#pragma once

#include "Scene.h"
#include "../common/VulkanHelper.h"

class VulkanApp;

class RenderSchemeFactory
{
public:
    RenderSchemeFactory();
    virtual ~RenderSchemeFactory();

    virtual void Setup();
    virtual void Update();
    virtual void Render(VkCommandBuffer& p_CommandBuffer);
    virtual void Prepare(Scene* p_Scene);
    virtual void UpdateNodeList(Node* p_Parent);
    virtual void ResizeWindow(VkCommandBuffer& p_CommandBuffer);

    GETSET(glm::mat4x4, ProjectViewMatrix);
    GETSET(VulkanApp*, VulkanApplication)

protected:
    QMap<QString, QPair<VkPipeline, VkPipelineLayout> > m_GraphicsPipelineMap;
};
