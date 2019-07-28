#pragma once
#include "../../../common/VulkanApp.h"

/*********** ASSIMP HEADER FILES ***********/
#include <assimp/scene.h>     
#include <assimp/Importer.hpp> 
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

class SimpleMesh : public DrawableInterface
{
public:
    SimpleMesh(VulkanApp* p_VulkanApp);
    virtual ~SimpleMesh();

	void Setup();
	void Update();
	void LoadMesh(const char* p_Filename, bool p_UseStaging = true);

    void ResizeWindow(int width, int height);

private:
	void CreateGraphicsPipeline();
	void RecordCommandBuffer();
	void CreateCommandBuffers();

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

	class MeshNode;
	void LoadNode(MeshNode* p_MeshNode, const aiMesh* p_pAiMesh);
	bool Load(const char* p_Filename);

	struct {
		glm::mat4 NormalMatrix;
		glm::mat4 Projection;
		glm::mat4 View;
		glm::mat4 Model;
		glm::vec4 LightPosition;
	} TransformationUniforms;

	struct {
		VulkanBuffer					m_BufObj;
		VkDescriptorBufferInfo			m_DescriptorBufInfo;// Descriptor buffer info that need to supplied into write descriptor set (VkWriteDescriptorSet)
		std::vector<VkMappedMemoryRange>m_MappedRange;		// Metadata of memory mapped objects
		uint8_t*						m_MappedMemory;  	// Host pointer containing the mapped device address which is used to write data into.
		size_t							m_DataSize;			// Data size.
	} UniformBuffer;

	// List of all the VkDescriptorSetLayouts 
	std::vector<VkDescriptorSetLayout> descLayout;

	// Decriptor pool object that will be used for allocating VkDescriptorSet object
	VkDescriptorPool descriptorPool;

	// List of all created VkDescriptorSet
	std::vector<VkDescriptorSet> descriptorSet;

	// Mesh data structures
    // GPU Data structure for Meshes contain device buffer and memory
    struct Mesh {
        VulkanBuffer vertexBuffer;
        VulkanBuffer indexBuffer;
        uint32_t indexCount;
    } m_Mesh;

    // CPU Data structure for Meshes
	struct Vertex
	{
		Vertex(const glm::vec3& p_Pos, const glm::vec3& p_Normals) {
			m_Pos = p_Pos; m_Normals = p_Normals;
		}

		glm::vec3 m_Pos;
		glm::vec3 m_Normals;
	};
    struct MeshNode {
        std::vector<Vertex> Vertices;
        std::vector<unsigned int> Indices;
    };

	std::vector<MeshNode> m_Nodes;
    Assimp::Importer m_AssimpImporter;
    const aiScene* m_pMeshScene;

	// Vertex buffer specific objects
	VkVertexInputBindingDescription		m_VertexInputBinding;
	VkVertexInputAttributeDescription	m_VertexInputAttribute[2]; // Why 2 - for position and normal

    // Store app specific objects
    VkPipelineLayout m_hPipelineLayout;
    VkPipeline       m_hGraphicsPipeline;
    VulkanApp*		 m_VulkanApplication;
	float rotation;
};
