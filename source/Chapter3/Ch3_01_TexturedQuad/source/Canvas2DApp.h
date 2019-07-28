#pragma once
#include "../../../common/VulkanApp.h"
#include "Quad.h"


class Canvas2DApp :public VulkanApp
{
public:
    Canvas2DApp();
    ~Canvas2DApp();

    void Configure();
    void Setup();
    void Update();
    bool Render();

private:
    Quad*                       mp_Quad;

    VkPipelineLayout            m_canvas2DPipelineLayout;
    VkPipeline                  m_canvas2DGraphicsPipeline;

    VkDescriptorSetLayout       m_descSetLayout;
    VkDescriptorPool            m_descriptorPool;

    VkSampler                   m_textureSampler;

    void CreateDescriptorLayout();
    bool CreateGraphicsPipeline();
    void CreateDescriptorPool();
    void CreateTexturedQuad();
    void CreateTextureSampler();
    void BuildCommandBuffers();

    void CreateDescriptorSet(VkImageView imageView, VkDescriptorSet& descSet);
};
