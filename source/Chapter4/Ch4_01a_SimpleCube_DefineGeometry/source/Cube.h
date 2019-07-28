#pragma once
#include "../../../common/VulkanApp.h"

struct Vertex
{
    glm::vec3 m_Position; // Vertex Position => x, y, z
    glm::vec3 m_Color;    // Color format => r, g, b
};

const float dimension = 0.75f;
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

class Cube : public DrawableInterface
{
public:
    Cube(VulkanApp * p_VulkanApp);
    virtual ~Cube();

	void Setup();
	void Update();

private:
	void CreateGraphicsPipeline();
	void RecordCommandBuffer();
	void CreateVertexBuffer(const void *vertexData, uint32_t dataSize, uint32_t dataStride);
	void CreateCommandBuffers(); // Overide the default implementation as per application requirement

	struct {
		VulkanBuffer m_BufObj;
	} VertexBuffer;

	// Vertex buffer specific objects
	VkVertexInputBindingDescription		m_VertexInputBinding;
	VkVertexInputAttributeDescription	m_VertexInputAttribute[2]; // Why 2 - for position and color attribute

    // Store app specific objects
    VkPipelineLayout m_hPipelineLayout;
    VkPipeline       m_hGraphicsPipeline;
    VulkanApp*		 m_VulkanApplication;
};
