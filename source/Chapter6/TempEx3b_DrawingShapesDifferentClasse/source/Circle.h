#pragma once
#include "../../../common/VulkanApp.h"
#include "Rect.h"

#include "../TempEx2_SceneGraph/Transformation.h"
#include "../TempEx2_SceneGraph/Scene.h"
#include "../TempEx2_SceneGraph/Node.h"

class CircleMultiDrawFactory;

class Circle : public Node
{
public:
    enum DRAW_TYPE
    {
        FILLED = 0,
        OUTLINE,
        ROUNDED,
        DRAW_TYPE_COUNT
    };

public:
    Circle(Scene* p_Scene, Node* p_Parent, const BoundingRegion& p_BoundedRegion, const QString& p_Name = "");
    Circle(Scene* p_Scene, Node* p_Parent, glm::vec2 m_Center, float radius, const QString& p_Name = "");
    virtual ~Circle() {}

    GETSET(DRAW_TYPE, DrawType)
        
protected:
    virtual void Update(Node* p_Item = NULL)
    {
        Node::Update(p_Item);

        CreateCircleVertexBuffer();
    }

public:
    void UpdateMeAndMyChildren()
    {
        Update(this);
    }

    RenderSchemeFactory* GetRenderSchemeFactory();

    virtual void Setup();
    void CreateCircleVertexBuffer();
    VulkanBuffer m_VertexBuffer;
};

struct CircleDescriptorSet : public RectangleDescriptorSet
{
    CircleDescriptorSet(VulkanApp* p_VulkanApplication)
        : RectangleDescriptorSet(p_VulkanApplication) {}
};

class CircleMultiDrawFactory : public RenderSchemeFactory
{
public:
    CircleMultiDrawFactory(VulkanApp* p_VulkanApp);
    virtual ~CircleMultiDrawFactory();

public:
    virtual void Setup();
    virtual void Update();
    virtual void Render(VkCommandBuffer& p_CommandBuffer);

    void ResizeWindow(VkCommandBuffer& p_CommandBuffer);
    virtual void Prepare(Scene* p_Scene);

private:
    void CreateGraphicsPipeline(bool p_ClearGraphicsPipelineMap = false);
    void CreateCircleFillPipeline();
    void CreateCircleOutlinePipeline();
    void CreateVertexLayoutBinding();

    void createPushConstants();


    virtual void UpdateNodeList(Node* p_Item);

    enum CIRCLE_GRAPHICS_PIPELINES
    {
        PIPELINE_FILLED = 0,
        PIPELINE_OUTLINE,
        PIPELINE_COUNT,
    };

    std::vector<VkVertexInputBindingDescription>   m_VertexInputBinding[PIPELINE_COUNT];   // 0 for (position and color) 1 for ()
    std::vector<VkVertexInputAttributeDescription> m_VertexInputAttribute[PIPELINE_COUNT]; // Why 7 = 2(for position and color) + 5 (transform and rotation) + Color

    typedef std::vector<Node*> NodeVector;
    NodeVector m_PipelineTypeModelVector[PIPELINE_COUNT];

    std::shared_ptr<CircleDescriptorSet> m_DescriptorSet;
};
