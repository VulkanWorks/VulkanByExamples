#include "Quad.h"

Quad::Quad(VulkanApp * p_VulkanApp)
    : m_pVulkanApp(p_VulkanApp)
{
    m_VertexBuffer.m_Buffer = nullptr;
    m_VertexBuffer.m_Memory = nullptr;
    m_TextureImage = {};
    m_TextureImageView = {};
    m_DescriptorSet = nullptr;
}

Quad::~Quad()
{
    assert(m_pVulkanApp->m_hDevice);

    if (m_VertexBuffer.m_Buffer != nullptr)
    {
        vkDestroyBuffer(m_pVulkanApp->m_hDevice, m_VertexBuffer.m_Buffer, nullptr);
        m_VertexBuffer.m_Buffer = nullptr;
    }

    if (m_VertexBuffer.m_Memory != nullptr)
    {
        vkFreeMemory(m_pVulkanApp->m_hDevice, m_VertexBuffer.m_Memory, nullptr);
        m_VertexBuffer.m_Memory = nullptr;
    }

    if (m_TextureImageView.imageView != nullptr)
    {
        vkDestroyImageView(m_pVulkanApp->m_hDevice, m_TextureImageView.imageView, nullptr);
        m_TextureImageView.imageView = nullptr;
    }

    if (m_TextureImage.out.image != nullptr)
    {
        vkDestroyImage(m_pVulkanApp->m_hDevice, m_TextureImage.out.image, nullptr);
        m_TextureImage.out.image = nullptr;
    }

    if (m_TextureImage.out.deviceMemory)
    {
        vkFreeMemory(m_pVulkanApp->m_hDevice, m_TextureImage.out.deviceMemory, nullptr);
        m_TextureImage.out.deviceMemory = nullptr;
    }
}

Quad* Quad::CreateTexturedQuad(VulkanApp * p_pVulkanApp, const void* p_pPixels, int Width, int Height, Vertex* p_pVertices)
{
    Quad* pQuad = new Quad(p_pVulkanApp);

    assert(pQuad);

    // Create vertex buffer for QUAD
    pQuad->m_VertexBuffer.m_DataSize = sizeof(Vertex)*4;
    pQuad->m_VertexBuffer.m_MemoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VulkanHelper::CreateBuffer(p_pVulkanApp->m_hDevice, p_pVulkanApp->m_physicalDeviceInfo.memProp, pQuad->m_VertexBuffer, NULL, p_pVertices);

    pQuad->m_TextureImage.in.extent = { static_cast<uint32_t>(Width), static_cast<uint32_t>(Height), 1 };

    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent = pQuad->m_TextureImage.in.extent;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;

    //VK_IMAGE_USAGE_TRANSFER_SRC_BIT and/or VK_IMAGE_USAGE_TRANSFER_DST_BIT
    // MOLTEN VK does not support VK_IMAGE_TILING_LINEAR
#ifdef _WIN32
    imageInfo.tiling = VK_IMAGE_TILING_LINEAR;  // Linear tiling
#elif __APPLE__
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;  // Optimal tiling
#endif

    imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    pQuad->m_TextureImage.in.memoryFlags = (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // 2: Create the Vulkan image
    VulkanHelper::CreateImage(p_pVulkanApp->m_hDevice, p_pVulkanApp->m_physicalDeviceInfo.memProp, pQuad->m_TextureImage, &imageInfo);

    // 3: Copy the pixel content to the image
    pQuad->LinearCopyPixelsToImage(p_pPixels);

    // 4: Create image view
    pQuad->m_TextureImageView.pImage = &pQuad->m_TextureImage.out.image;
    VulkanHelper::CreateImageView(p_pVulkanApp->m_hDevice, pQuad->m_TextureImageView);

    return (pQuad);
}

void Quad::LinearCopyPixelsToImage(const void* p_pData)
{
    int imageSize = m_TextureImage.in.extent.width * m_TextureImage.in.extent.height * 4;
    int strideSize = (m_TextureImage.in.extent.width * 4);

    VkImageSubresource subRes = {};
    subRes.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subRes.mipLevel = 0;
    subRes.arrayLayer = 0;

    VkSubresourceLayout subResLayout;
    vkGetImageSubresourceLayout(m_pVulkanApp->m_hDevice, m_TextureImage.out.image, &subRes, &subResLayout);

    void* pHostMem = nullptr;
    vkMapMemory(m_pVulkanApp->m_hDevice, m_TextureImage.out.deviceMemory, 0, imageSize, 0, &pHostMem);
    assert(pHostMem);

    if (pHostMem)
    {
        unsigned char* pSrc = (unsigned char*)p_pData;
        unsigned char* pDst = (unsigned char*)pHostMem;

        for (uint32_t j = 0; j < m_TextureImage.in.extent.height; j++)
        {
            pSrc = (unsigned char*)p_pData + j*strideSize;
            pDst = (unsigned char*)pHostMem + j*subResLayout.rowPitch;

            memcpy(pDst, pSrc, (size_t)strideSize);
        }
    }
    vkUnmapMemory(m_pVulkanApp->m_hDevice, m_TextureImage.out.deviceMemory);
}


void Quad::Draw(VkCommandBuffer p_CmdBuffer)
{
    VkBuffer vertexBuffers[] = { m_VertexBuffer.m_Buffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(p_CmdBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdDraw(p_CmdBuffer, 4, 1, 0, 0);
}
