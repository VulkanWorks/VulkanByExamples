#pragma once
#include "../../../common/VulkanApp.h"

#include "../SceneGraph/Transformation3D.h"
#include "../SceneGraph/Scene3D.h"
#include "../SceneGraph/Model3D.h"

struct Vertex
{
    glm::vec3 m_Position; // Vertex Position => x, y, z
    glm::vec3 m_Color;    // Color format => r, g, b
};

class RectangleModel : public Model3D
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
    RectangleModel(VulkanApp* p_VulkanApp/*REMOVE ME*/, Scene3D* p_Scene, Model3D* p_Parent, const QString& p_Name = "", SHAPE p_ShapeType = SHAPE::SHAPE_NONE, RENDER_SCEHEME_TYPE p_RenderSchemeType = RENDER_SCEHEME_INSTANCED);
    virtual ~RectangleModel() {}
    GETSET(DRAW_TYPE, DrawType)

    virtual AbstractModelFactory* GetRenderScemeFactory();
};

struct RectangleDescriptorSet
{
    struct UniformBufferObj {
        UniformBufferObj()
        {
            memset(this, 0, sizeof(UniformBufferObj));
        }

        VulkanBuffer					m_BufObj;
        VkDescriptorBufferInfo			m_DescriptorBufInfo;// Descriptor buffer info that need to supplied into write descriptor set (VkWriteDescriptorSet)
        std::vector<VkMappedMemoryRange>m_MappedRange;		// Metadata of memory mapped objects
        uint8_t*						m_MappedMemory;  	// Host pointer containing the mapped device address which is used to write data into.
        size_t							m_DataSize;			// Data size.
    };

    RectangleDescriptorSet(VulkanApp* p_VulkanApplication)
    {
        m_VulkanApplication = p_VulkanApplication;

        UniformBuffer = new UniformBufferObj;
    }
    ~RectangleDescriptorSet()
    {
        //// Destroy descriptors
        //for (int i = 0; i < descLayout.size(); i++) {
        //	vkDestroyDescriptorSetLayout(m_VulkanApplication->m_hDevice, descLayout[i], NULL);
        //}
        //descLayout.clear();

        //vkFreeDescriptorSets(m_VulkanApplication->m_hDevice, descriptorPool, (uint32_t)descriptorSet.size(), &descriptorSet[0]);
        //vkDestroyDescriptorPool(m_VulkanApplication->m_hDevice, descriptorPool, NULL);
    }

    void CreateUniformBuffer();
    void DestroyUniformBuffer();

    void CreateDescriptorSetLayout();
    void DestroyDescriptorLayout();

    void CreateDescriptor();

    // Creates the descriptor pool, this function depends on - 
    void CreateDescriptorPool();
    // Creates the descriptor sets using descriptor pool.
    // This function depend on the createDescriptorPool() and createUniformBuffer().
    void CreateDescriptorSet();

    // List of all the VkDescriptorSetLayouts 
    std::vector<VkDescriptorSetLayout> descLayout;

    // Decriptor pool object that will be used for allocating VkDescriptorSet object
    VkDescriptorPool descriptorPool;

    // List of all created VkDescriptorSet
    std::vector<VkDescriptorSet> descriptorSet;

    VulkanApp*		 m_VulkanApplication;
    UniformBufferObj* UniformBuffer;
};

class RectangleFactory : public AbstractModelFactory
{
public:
    RectangleFactory(VulkanApp* p_VulkanApp);
    virtual ~RectangleFactory();

public:
	virtual void Setup();
    virtual void Update();
    virtual void Render() { RecordCommandBuffer(); }

    void ResizeWindow(int width, int height);
//	virtual void Prepare(Scene3D* p_Scene);

private:
    void CreateGraphicsPipeline(bool p_ClearGraphicsPipelineMap = false);
    void CreateRectFillPipeline();
    void CreateRectOutlinePipeline();

    void RecordCommandBuffer();
    void CreateVertexBuffer();

    void Render(VkCommandBuffer& p_CmdBuffer);

    virtual void UpdateModelList(Model3D* p_Item);

    QMap<QString, QPair<VkPipeline, VkPipelineLayout> > m_GraphicsPipelineMap;

    enum RECTANGLE_GRAPHICS_PIPELINES
    {
        PIPELINE_FILLED = 0,
        PIPELINE_OUTLINE,
        PIPELINE_COUNT,
    };

    std::vector<VkVertexInputBindingDescription>   m_VertexInputBinding[PIPELINE_COUNT];   // 0 for (position and color) 1 for ()
    std::vector<VkVertexInputAttributeDescription> m_VertexInputAttribute[PIPELINE_COUNT]; // Why 7 = 2(for position and color) + 5 (transform and rotation) + Color

    VulkanApp* m_VulkanApplication;

    ////////////////////////////////////////////////////////////////
public:
    void PrepareInstanceData();


    // Per-instance data block
    struct InstanceData {
        glm::mat4 m_Model;
        glm::vec4 m_Rect;
        glm::vec4 m_Color;
    };


    VulkanBuffer m_VertexBuffer[PIPELINE_COUNT];
    int m_VertexCount[PIPELINE_COUNT];
    typedef std::vector<Model3D*> ModelVector;
    VulkanBuffer m_InstanceBuffer[PIPELINE_COUNT];
    ModelVector m_PipelineTypeModelVector[PIPELINE_COUNT];
    int m_OldInstanceDataSize[PIPELINE_COUNT];
};


//TODO: 
//a. VertexBuffer should be m_VertexBuffer[PIPELINE_COUNT]
//b. Address its distruction.
//c. Assign correct vertex buffer data as per pipeline rectFilledVertices, rectOutlineVertices

class RectangleModel;
class QMouseEvent;
class ProgressBar : public Model3D
{
public:
    ProgressBar(Scene3D* p_Scene, Model3D* p_Parent, const QString& p_Name = "", SHAPE p_ShapeType = SHAPE::SHAPE_NONE);
    virtual ~ProgressBar() {}

    virtual bool mouseMoveEvent(QMouseEvent* p_Event);
    RectangleModel* progressIndicator;
    RectangleModel* bar;
};

class AudioMixerItem : public Model3D
{
public:
    AudioMixerItem(Scene3D* p_Scene, Model3D* p_Parent, const QString& p_Name, glm::vec2 p_TopLeftPos, glm::vec2 p_Dim, SHAPE p_ShapeType = SHAPE::SHAPE_NONE);
    virtual ~AudioMixerItem() {}

    //virtual bool mouseMoveEvent(QMouseEvent* p_Event);
    //Model3D* progressIndicator;
};

