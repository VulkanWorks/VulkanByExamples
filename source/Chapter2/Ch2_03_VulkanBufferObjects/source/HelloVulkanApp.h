#pragma once
#include "../../../common/VulkanApp.h"

struct Position { float x, y, z, w; }; // Vertex Position
struct Color{ float r, g, b, a; }; // Color format Red, Green, Blue, Alpha
struct Vertex
{
	Position m_Position;
	Color m_Color;
};

static const Vertex s_TriangleVertices[3] =
{
	{ Position{ 0.0f, 1.0f, 0.0f, 1.0f}, Color{ 1.0f, 0.0f, 0.0f, 1.0} },
	{ Position{ 1.0f, -1.0f, 0.0f, 1.0f}, Color{ 0.0f, 0.0f, 1.0f, 1.0} },
	{ Position{ -1.0f, -1.0f, 0.0f, 1.0f}, Color{ 0.0f, 1.0f, 0.0f, 1.0} },
};

struct {
	VkBuffer m_Buffer;
	VkDeviceMemory m_Memory;
	VkDescriptorBufferInfo m_BufferInfo;
} VertexBuffer;

class HelloVulkanApp : public VulkanApp
{
public:
    HelloVulkanApp();
	virtual ~HelloVulkanApp();

	void SetApplicationName(string name);

protected:
	virtual void Configure();
	virtual void Setup();
	virtual void Update();

private:
	void CreateGraphicsPipeline();
	void RecordCommandBuffer();
	void CreateVertexBuffer(const void *vertexData, uint32_t dataSize, uint32_t dataStride);

	// Vertex buffer specific objects
	VkVertexInputBindingDescription		m_VertexInputBinding;
	VkVertexInputAttributeDescription	m_VertexInputAttribute[2]; // Why 2 - for position and color attribute

    // Store app specific objects
    VkPipelineLayout m_hPipelineLayout;
    VkPipeline       m_hGraphicsPipeline;
};