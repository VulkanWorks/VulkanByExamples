#pragma once

#include "Scene.h"
#include "../common/VulkanHelper.h"

class VulkanApp;

// Rename to RenderSchemeBase
class RenderSchemeFactory
{
public:
    RenderSchemeFactory();
    virtual ~RenderSchemeFactory();

    virtual void Setup();
    virtual void Update();
    virtual void UpdateUniform();
    virtual void Render(VkCommandBuffer& p_CommandBuffer);
    virtual void UpdateDirty(); // XXXXXX
    virtual void Prepare(Scene* p_Scene);
    virtual void UpdateNodeList(Node* p_Item);
    virtual void RemoveNodeList(Node* p_Item);
    virtual void ResizeWindow(VkCommandBuffer& p_CommandBuffer);

    GETSET(glm::mat4x4, ProjectViewMatrix);
    GETSET(VulkanApp*, VulkanApplication)
    GETSET(unsigned int, MemPoolIdx)

protected:
    QMap<QString, QPair<VkPipeline, VkPipelineLayout> > m_GraphicsPipelineMap;
};
