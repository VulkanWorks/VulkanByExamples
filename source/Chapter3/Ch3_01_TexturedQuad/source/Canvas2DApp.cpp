#include "Canvas2DApp.h"
#include <QDirIterator>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

Canvas2DApp::Canvas2DApp():
    m_canvas2DPipelineLayout(VK_NULL_HANDLE), 
    m_canvas2DGraphicsPipeline(VK_NULL_HANDLE), 
    m_textureSampler(VK_NULL_HANDLE),
    mp_Quad(nullptr)
{
}

Canvas2DApp::~Canvas2DApp()
{
    assert(mp_Quad);

    delete mp_Quad;

    vkDestroySampler(m_hDevice, m_textureSampler, nullptr);

    vkDestroyDescriptorSetLayout(m_hDevice, m_descSetLayout, nullptr);

    vkDestroyDescriptorPool(m_hDevice, m_descriptorPool, nullptr);
    vkDestroyPipeline(m_hDevice, m_canvas2DGraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(m_hDevice, m_canvas2DPipelineLayout, nullptr);
}

void Canvas2DApp::Configure()
{
    SetApplicationName("Canvas2D");
    SetWindowDimension(800, 600);

    VulkanHelper::GetInstanceLayerExtensionProperties();

#ifdef _WIN32
    // Add Validation Layers
    AddValidationLayer("VK_LAYER_LUNARG_standard_validation");

    // Add Vulkan instance extensions
    AddInstanceExtension(VK_KHR_SURFACE_EXTENSION_NAME);
    AddInstanceExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    AddInstanceExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

   #ifdef _WIN64
   #else
   #endif
#elif __APPLE__
    AddInstanceExtension("VK_KHR_surface");
    AddInstanceExtension("VK_MVK_macos_surface");
#endif
}

void Canvas2DApp::Update()
{

}

void Canvas2DApp::Setup()
{
    CreateCommandBuffers();
    CreateTextureSampler();
    CreateDescriptorLayout();
    CreateDescriptorPool();
    CreateTexturedQuad();
    CreateGraphicsPipeline();
    BuildCommandBuffers();
}

// Create an instance of Quad object for each image
// file found in the executable folder
void Canvas2DApp::CreateTexturedQuad()
{
    // Initialize vertex positions for the quad in normalized device coordinates
    Vertex vertices[] =
    {
        { { -1.0f, -1.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f,0.0f } }, // Top left
        { { 1.0f, -1.0f },{ 0.0f, 0.0f, 1.0f },{ 1.0f,0.0f } }, // Top right
        { { -1.0f, 1.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f,1.0f } }, // Bottom left
        { { 1.0f, 1.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f,1.0f } }  // Bottom right
    };

    // 1: Load the jpg file and retrieve the pixel content.
    int texWidth, texHeight, texChannels;

#ifdef _WIN32
    string filename = "../../../resources/textures/photos/1.jpg";
#elif __APPLE__
    string filename = "/Users/parminder/Dev/Metal/QtMetal/VulkanByExample-master/source/resources/textures/photos/1.jpg";
#endif

    stbi_uc* pPixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    if (pPixels)
    {
        mp_Quad = Quad::CreateTexturedQuad(this, pPixels, texWidth, texHeight, vertices);

        // Create descriptor for QUAD
        CreateDescriptorSet(mp_Quad->m_TextureImageView.imageView, mp_Quad->m_DescriptorSet);

        stbi_image_free(pPixels);
    }
}

void Canvas2DApp::CreateDescriptorSet(VkImageView imageView, VkDescriptorSet& descSet)
{
    // Allocate descriptor set
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_descSetLayout;

    VkResult result = vkAllocateDescriptorSets(m_hDevice, &allocInfo, &descSet);
    assert(result == VK_SUCCESS);
    
    // Setup image info for the texture
    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout =  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = imageView;
    imageInfo.sampler = m_textureSampler;

    VkWriteDescriptorSet descriptorWrites = {};

    // Setup descriptor write for image info
    descriptorWrites.sType =  VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites.dstSet = descSet;
    descriptorWrites.dstBinding = 0;
    descriptorWrites.dstArrayElement = 0;
    descriptorWrites.descriptorType =  VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites.descriptorCount = 1;
    descriptorWrites.pImageInfo = &imageInfo;

    // Update the descriptor set
    uint32_t count = sizeof(descriptorWrites) / sizeof(VkWriteDescriptorSet);
    vkUpdateDescriptorSets(m_hDevice, count, &descriptorWrites, 0, nullptr);
}

void Canvas2DApp::BuildCommandBuffers()
{
    // The following code records the commands to draw
    // a quad in a blue background

    // The color values are defined in this order (Red,Green,Blue,Alpha)
    VkClearValue clearColor = { 1.0f, 0.0f, 0.0f, 1.0f };
    // Offset to render in the frame buffer
    VkOffset2D   renderOffset = { 0, 0 };
    // Width & Height to render in the frame buffer
    VkExtent2D   renderExtent = m_swapChainExtent;

    // For each command buffers in the command buffer list
    for (size_t i = 0; i < m_hCommandBufferList.size(); i++)
    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        // Indicate that the command buffer can be resubmitted to the queue
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        // Step 1: Begin command buffer
        vkBeginCommandBuffer(m_hCommandBufferList[i], &beginInfo);

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_hRenderPass;
        renderPassInfo.framebuffer = m_hFramebuffers[i];
        renderPassInfo.renderArea.offset = renderOffset;
        renderPassInfo.renderArea.extent = renderExtent;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        // Step 2: Begin render pass
        vkCmdBeginRenderPass(m_hCommandBufferList[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Step 3: Bind graphics pipeline
        vkCmdBindPipeline(m_hCommandBufferList[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_canvas2DGraphicsPipeline);

        // Step 4: Bind descriptor set
        vkCmdBindDescriptorSets(m_hCommandBufferList[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_canvas2DPipelineLayout, 0, 1, &(mp_Quad->m_DescriptorSet), 0, nullptr);

        // Step 5: Draw the Quad
        mp_Quad->Draw(m_hCommandBufferList[i]);

        // Step 6: End the Render pass
        vkCmdEndRenderPass(m_hCommandBufferList[i]);

        // Step 7: End the Command buffer
        VkResult result = vkEndCommandBuffer(m_hCommandBufferList[i]);
        assert(result == VK_SUCCESS);
    }
}

void Canvas2DApp::CreateDescriptorLayout()
{
    // Setup binding for uniform buffer for Vertex shader
    // and sampler for the fragment shader
    VkDescriptorSetLayoutBinding bindings;

    // Initialize the layout binding for the sampler
    VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
    bindings.binding = 0;
    bindings.descriptorCount = 1;
    bindings.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings.pImmutableSamplers = nullptr;
    bindings.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    // Create the descriptor set layout
    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = sizeof(bindings)/ sizeof(VkDescriptorSetLayoutBinding);
    layoutInfo.pBindings = &bindings;

    VkResult result = vkCreateDescriptorSetLayout(m_hDevice,
                                        &layoutInfo,
                                        nullptr,
                                        &m_descSetLayout);
    assert(result == VK_SUCCESS);
}

bool Canvas2DApp::Render()
{ 
    return VulkanApp::Render(); 
}

bool Canvas2DApp::CreateGraphicsPipeline()
{
    bool result = true;

    // Compile the vertex shader
#ifdef _WIN32
    VkShaderModule vertShader = VulkanHelper::CreateShader(m_hDevice,"../source/shaders/QuadVert.spv");
#elif __APPLE__
    VkShaderModule vertShader = VulkanHelper::CreateShaderFromQRCResource(m_hDevice, "://source/shaders/QuadVert.spv"); // Relative path to binary output dir
#endif

    // Setup the vertex shader stage create info structures
    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShader;
    vertShaderStageInfo.pName = "main";

    // Compile the fragment shader
#ifdef _WIN32
    VkShaderModule fragShader = VulkanHelper::CreateShader(m_hDevice,"../source/shaders/QuadFrag.spv");
#elif __APPLE__
    VkShaderModule fragShader = VulkanHelper::CreateShaderFromQRCResource(m_hDevice, "://source/shaders/QuadFrag.spv"); // Relative path to binary output dir
#endif

    // Setup the fragment shader stage create info structures
    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShader;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    
    // Indicates the rate at which the information will be
    // injected for vertex input.
    VkVertexInputBindingDescription     VertexInputBinding;
    VkVertexInputAttributeDescription   VertexInputAttribute[3];

    VertexInputBinding.binding = 0;
    VertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    VertexInputBinding.stride = sizeof(Vertex);

    // The VkVertexInputAttribute interpreting the data.
    VertexInputAttribute[0].binding = 0;
    VertexInputAttribute[0].location = 0;
    VertexInputAttribute[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    VertexInputAttribute[0].offset = offsetof(struct Vertex, m_Position);

    VertexInputAttribute[1].binding = 0;
    VertexInputAttribute[1].location = 1;
    VertexInputAttribute[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    VertexInputAttribute[1].offset = offsetof(struct Vertex, m_Color);

    // Texture coordinate
    VertexInputAttribute[2].binding = 0;
    VertexInputAttribute[2].location = 2;
    VertexInputAttribute[2].format = VK_FORMAT_R32G32_SFLOAT;
    VertexInputAttribute[2].offset = offsetof(Vertex, m_TexCoord);

    // Setup the vertex input
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = (sizeof(VertexInputAttribute) / sizeof(VkVertexInputAttributeDescription));
    vertexInputInfo.pVertexBindingDescriptions = &VertexInputBinding;
    vertexInputInfo.pVertexAttributeDescriptions = &VertexInputAttribute[0];


    // Setup input assembly
    // We will be rendering 2 triangle using triangle strip topology
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Setup viewport to the maximum widht and height of the window
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)m_swapChainExtent.width;
    viewport.height = (float)m_swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    // Setup scissor rect
    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = m_swapChainExtent;

    // Setup view port state
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // Setup the rasterizer state
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    // Setup multi sampling. In our first example we will be using single sampling mode
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // Setup color output masks.
    // Set to write out RGBA components
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    // Setup color blending
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    // Create pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pSetLayouts = &m_descSetLayout;

    VkResult vkResult = vkCreatePipelineLayout(m_hDevice, &pipelineLayoutInfo, nullptr, &m_canvas2DPipelineLayout);
    assert(vkResult == VK_SUCCESS);

    // Create graphics pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = m_canvas2DPipelineLayout;
    pipelineInfo.renderPass = m_hRenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    vkResult = vkCreateGraphicsPipelines(m_hDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_canvas2DGraphicsPipeline);
    assert(vkResult == VK_SUCCESS);

    // Cleanup
    vkDestroyShaderModule(m_hDevice, fragShader, nullptr);
    vkDestroyShaderModule(m_hDevice, vertShader, nullptr);

    return (result);
}

void Canvas2DApp::CreateDescriptorPool() 
{
    VkDescriptorPoolSize poolSizes = {};

    poolSizes.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes.descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = sizeof(poolSizes) / sizeof(VkDescriptorPoolSize);
    poolInfo.pPoolSizes = &poolSizes;
    poolInfo.maxSets = 1;

    VkResult result = vkCreateDescriptorPool(m_hDevice, &poolInfo, nullptr, &m_descriptorPool);
    assert(result == VK_SUCCESS);
}

void Canvas2DApp::CreateTextureSampler()
{
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 1;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    VkResult result = vkCreateSampler(m_hDevice, &samplerInfo, nullptr, &m_textureSampler);

    assert (result == VK_SUCCESS);
}

int main(int argc, char **argv)
{
    QApplication qtApp(argc, argv);

    Canvas2DApp* pCanvas2DApp = new Canvas2DApp(); // Create Vulkan app instance
    pCanvas2DApp->EnableDepthBuffer(false);
    pCanvas2DApp->Initialize();
    pCanvas2DApp->m_pWindow->show();
    qtApp.exec();

    delete pCanvas2DApp;
    return 0;
}
