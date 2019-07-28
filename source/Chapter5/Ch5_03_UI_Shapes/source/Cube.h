#pragma once
#include "../../../common/VulkanApp.h"

#include "../../../common/SceneGraph/Transformation3D.h"
#include "../../../common/SceneGraph/Scene3D.h"
#include "../../../common/SceneGraph/Model3D.h"

struct Vertex
{
    glm::vec3 m_Position; // Vertex Position => x, y, z
    glm::vec3 m_Color;    // Color format => r, g, b
};

static const Vertex cubeVertices[] =
{
    { glm::vec3(1, 0, -0),	glm::vec3(0.f, 0.f, 0.f) },
    { glm::vec3(0, 0, -0),	glm::vec3(1.f, 0.f, 0.f) },
    { glm::vec3(1, 1, -0),	glm::vec3(0.f, 1.f, 0.f) },
    { glm::vec3(1, 1, -0),	glm::vec3(0.f, 1.f, 0.f) },
    { glm::vec3(0, 0, -0),	glm::vec3(1.f, 0.f, 0.f) },
    { glm::vec3(0, 1, -0),	glm::vec3(1.f, 1.f, 0.f) },
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

struct CubeDescriptorSet
{
    struct  UniformBufferObj {
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

    CubeDescriptorSet(VulkanApp* p_VulkanApplication)
    {
        m_VulkanApplication = p_VulkanApplication;

        UniformBuffer = new UniformBufferObj;
    }
    ~CubeDescriptorSet()
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
    void Setup();
    void Update();

private:
    void CreateGraphicsPipeline(bool p_ClearGraphicsPipelineMap = false);
        void CreateRectFillPipeline();
        void CreateRectOutlinePipeline();
    void CreateVertexBuffer(const void *vertexData, uint32_t dataSize, uint32_t dataStride);

    void RecordCommandBuffer(); // made public

public:
    VulkanBuffer m_VertexBuffer, m_InstanceBuffer;

    void PrepareInstanceData();
    virtual void UpdateModelList(Model3D* p_Item)
    {
        m_ModelList.push_back(p_Item);
    }

    // Vertex buffer specific objects
    VkVertexInputBindingDescription		m_VertexInputBinding[2];   // 0 for (position and color) 1 for ()
    VkVertexInputAttributeDescription	m_VertexInputAttribute[8]; // Why 7 = 2(for position and color) + 5 (transform and rotation) + Color

    VulkanApp*		 m_VulkanApplication;

    QMap<QString, QPair<VkPipeline, VkPipelineLayout> > m_GraphicsPipelineMap;

    //// Per-instance data block
    struct InstanceData {
        glm::mat4 m_Model;
        glm::vec4 m_Rect;
        glm::vec4 m_Color;
        //float scale;
        //uint32_t texIndex;
    };

    std::vector<InstanceData> m_InstanceData;
    typedef std::vector<Model3D*> ModelVector;
    ModelVector m_ModelList; // consider them as shared pointer
};
