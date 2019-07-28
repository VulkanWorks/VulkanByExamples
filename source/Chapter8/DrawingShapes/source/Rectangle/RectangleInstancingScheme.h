#pragma once
#include "../../../../common/VulkanApp.h"

#include "../../DrawingShapes_SceneGraph/RenderSchemeFactory.h"

#include "RectangleDescriptorSet.h"

class RectangleInstancingScheme : public RenderSchemeFactory
{
public:
    RectangleInstancingScheme(VulkanApp* p_VulkanApp);
    virtual ~RectangleInstancingScheme();

public:
    virtual void Setup();
    virtual void Update();
    virtual void UpdateDirty();
    virtual void Render() { RecordCommandBuffer(); }

    void ResizeWindow(int width, int height);

private:
    void CreateGraphicsPipeline(bool p_ClearGraphicsPipelineMap = false);
    void CreateRectFillPipeline();
    void CreateRectOutlinePipeline();

    void RecordCommandBuffer();
    void CreateVertexBuffer();

    void Render(VkCommandBuffer& p_CmdBuffer);

    virtual void UpdateNodeList(Node* p_Item);
    virtual void RemoveNodeList(Node* p_Item);

    enum RECTANGLE_GRAPHICS_PIPELINES
    {
        PIPELINE_FILLED = 0,
        PIPELINE_OUTLINE,
        PIPELINE_COUNT,
    };

    std::vector<VkVertexInputBindingDescription>   m_VertexInputBinding[PIPELINE_COUNT];   // 0 for (position and color) 1 for ()
    std::vector<VkVertexInputAttributeDescription> m_VertexInputAttribute[PIPELINE_COUNT]; // Why 7 = 2(for position and color) + 5 (transform and rotation) + Color

    ////////////////////////////////////////////////////////////////
public:
    void PrepareInstanceData(RECTANGLE_GRAPHICS_PIPELINES p_Pipeline = PIPELINE_COUNT);
    void UpdateDirtyInstanceData();

    // Per-instance data block
    struct InstanceData {
        glm::mat4 m_Model;
        glm::vec4 m_Rect;
        glm::vec4 m_Color;
        uint m_BoolFlags; // [0] Visibility [1] Unused [2] Unused [3] Unused
    };

    VulkanBuffer m_VertexBuffer[PIPELINE_COUNT];
    int m_VertexCount[PIPELINE_COUNT];
    typedef std::vector<Node*> ModelVector;
    VulkanBuffer m_InstanceBuffer[PIPELINE_COUNT];
    ModelVector m_PipelineTypeModelVector[PIPELINE_COUNT];
    int m_OldInstanceDataSize[PIPELINE_COUNT];

    std::shared_ptr<RectangleDescriptorSet> CDS = NULL;// std::make_shared<CubeDescriptorSet>(m_VulkanApplication);
    RectangleDescriptorSet::UniformBufferObj* UniformBuffer = NULL;
};
