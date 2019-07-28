#pragma once
#include "../../../common/VulkanApp.h"

struct Position { float x, y, z, w; };  // Vertex Position
struct Color { float r, g, b, a; };     // Color format Red, Green, Blue, Alpha
struct TexCoord { float u, v; };        // Texture coordinate (u,v)

struct Vertex
{
    Position    m_Position;
    Color       m_Color;
    TexCoord    m_TexCoord;
};

class Quad : public DrawableInterface
{
public:
    ~Quad();
    void Draw(VkCommandBuffer p_CmdBuffer);

    VulkanBuffer        m_VertexBuffer;
	VulkanImage			m_TextureImage;
	VulkanImageView     m_TextureImageView;
    VkDescriptorSet     m_DescriptorSet;
    VulkanApp*          m_pVulkanApp;

    static Quad* CreateTexturedQuad(VulkanApp * p_pVulkanApp, const void* p_pPixels, int Width, int Height, Vertex* p_pVertices);
    void LinearCopyPixelsToImage(const void* p_pData);
protected:
    Quad(VulkanApp * p_VulkanApp);
};

