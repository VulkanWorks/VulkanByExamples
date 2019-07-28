#include "RenderSchemeFactory.h"

RenderSchemeFactory::RenderSchemeFactory()
    : m_MemPoolIdx(0)
{
}

RenderSchemeFactory::~RenderSchemeFactory() {}

void RenderSchemeFactory::Setup() {}

void RenderSchemeFactory::Update() {}

void RenderSchemeFactory::UpdateUniform() {}

void RenderSchemeFactory::UpdateDirty() {}

void RenderSchemeFactory::Render(VkCommandBuffer &p_CommandBuffer) {}

void RenderSchemeFactory::Prepare(Scene *p_Scene) {}

void RenderSchemeFactory::UpdateNodeList(Node *p_Item) {}

void RenderSchemeFactory::RemoveNodeList(Node *p_Item) {}

void RenderSchemeFactory::ResizeWindow(VkCommandBuffer &p_CommandBuffer) {}
