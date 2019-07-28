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

const float dimension = 1.0f;
static const Vertex cubeVertices[] =
{
	{ glm::vec3(dimension, -dimension, -dimension),		glm::vec3(0.f, 0.f, 0.f) },
	{ glm::vec3(-dimension, -dimension, -dimension),	glm::vec3(1.f, 0.f, 0.f) },
	{ glm::vec3(dimension,  dimension, -dimension),		glm::vec3(0.f, 1.f, 0.f) },
	{ glm::vec3(dimension,  dimension, -dimension),		glm::vec3(0.f, 1.f, 0.f) },
	{ glm::vec3(-dimension, -dimension, -dimension),	glm::vec3(1.f, 0.f, 0.f) },
	{ glm::vec3(-dimension,  dimension, -dimension),	glm::vec3(1.f, 1.f, 0.f) },

	{ glm::vec3(dimension, -dimension, dimension),		glm::vec3(0.f, 0.f, 1.f) },
	{ glm::vec3(dimension,  dimension, dimension),		glm::vec3(0.f, 1.f, 1.f) },
	{ glm::vec3(-dimension, -dimension, dimension),		glm::vec3(1.f, 0.f, 1.f) },
	{ glm::vec3(-dimension, -dimension, dimension),		glm::vec3(1.f, 0.f, 1.f) },
	{ glm::vec3(dimension,  dimension, dimension),		glm::vec3(0.f, 1.f, 1.f) },
	{ glm::vec3(-dimension,  dimension, dimension),		glm::vec3(1.f, 1.f, 1.f) },

	{ glm::vec3(dimension, -dimension,  dimension),		glm::vec3(1.f, 1.f, 1.f) },
	{ glm::vec3(dimension, -dimension, -dimension),		glm::vec3(1.f, 1.f, 0.f) },
	{ glm::vec3(dimension,  dimension,  dimension),		glm::vec3(1.f, 0.f, 1.f) },
	{ glm::vec3(dimension,  dimension,  dimension),		glm::vec3(1.f, 0.f, 1.f) },
	{ glm::vec3(dimension, -dimension, -dimension),		glm::vec3(1.f, 1.f, 0.f) },
	{ glm::vec3(dimension,  dimension, -dimension),		glm::vec3(1.f, 0.f, 0.f) },

	{ glm::vec3(-dimension, -dimension,  dimension),	glm::vec3(0.f, 1.f, 1.f) },
	{ glm::vec3(-dimension, dimension, dimension),		glm::vec3(0.f, 0.f, 1.f) },
	{ glm::vec3(-dimension, -dimension, -dimension),	glm::vec3(0.f, 1.f, 0.f) },
	{ glm::vec3(-dimension, -dimension, -dimension),	glm::vec3(0.f, 1.f, 0.f) },
	{ glm::vec3(-dimension,  dimension,  dimension),	glm::vec3(0.f, 0.f, 1.f) },
	{ glm::vec3(-dimension,  dimension, -dimension),	glm::vec3(0.f, 0.f, 0.f) },

	{ glm::vec3(dimension, dimension, -dimension),		glm::vec3(1.f, 1.f, 1.f) },
	{ glm::vec3(-dimension, dimension, -dimension),		glm::vec3(0.f, 1.f, 1.f) },
	{ glm::vec3(dimension, dimension,  dimension),		glm::vec3(1.f, 1.f, 0.f) },
	{ glm::vec3(dimension, dimension,  dimension),		glm::vec3(1.f, 1.f, 0.f) },
	{ glm::vec3(-dimension, dimension, -dimension),		glm::vec3(0.f, 1.f, 1.f) },
	{ glm::vec3(-dimension, dimension,  dimension),		glm::vec3(0.f, 1.f, 0.f) },

	{ glm::vec3(dimension, -dimension, -dimension),		glm::vec3(1.f, 0.f, 1.f) },
	{ glm::vec3(dimension, -dimension,  dimension),		glm::vec3(1.f, 0.f, 0.f) },
	{ glm::vec3(-dimension, -dimension, -dimension),	glm::vec3(0.f, 0.f, 1.f) },
	{ glm::vec3(-dimension, -dimension, -dimension),	glm::vec3(0.f, 0.f, 1.f) },
	{ glm::vec3(dimension, -dimension,  dimension),		glm::vec3(1.f, 0.f, 0.f) },
	{ glm::vec3(-dimension, -dimension,  dimension),	glm::vec3(0.f, 0.f, 0.f) },
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

class Cube : public Model3D
{
};

class CubeFactory //: public DrawableInterface
{
private:
    CubeFactory(VulkanApp* p_VulkanApp);
    virtual ~CubeFactory();

public:
	void Setup();
	void Update();

private:
	void CreateGraphicsPipeline();
	void RecordCommandBuffer();
	void CreateVertexBuffer(const void *vertexData, uint32_t dataSize, uint32_t dataStride);

public:
    VulkanBuffer m_VertexBuffer, m_InstanceBuffer;

	////////////////////////////////////////////////////
	void prepareInstanceData(Scene3D* p_Scene);

	// Vertex buffer specific objects
	VkVertexInputBindingDescription		m_VertexInputBinding[2];   // 0 for (position and color) 1 for ()
	VkVertexInputAttributeDescription	m_VertexInputAttribute[7]; // Why 7 = 2(for position and color) + 5 (transform and rotation)

    // Store app specific objects
    VkPipelineLayout m_hPipelineLayout;
    VkPipeline       m_hGraphicsPipeline;
    VulkanApp*		 m_VulkanApplication;

	QMap<QString, VkPipeline> m_GraphicsPipelineMap;

	////////////////////////
	static CubeFactory* SingleTon(VulkanApp* p_VulkanApp) { return m_Singleton ? m_Singleton : (m_Singleton = new CubeFactory(p_VulkanApp)); }
	static CubeFactory* m_Singleton;
	
	// Per-instance data block
    struct InstanceData {
		glm::mat4 m_Model;
		//glm::vec3 pos;
		//glm::vec3 rot;
		//float scale;
		//uint32_t texIndex;
	};

	std::vector<glm::mat4> m_InstanceData;
};
