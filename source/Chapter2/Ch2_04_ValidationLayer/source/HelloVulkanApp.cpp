#include "HelloVulkanApp.h"

#include <QMainWindow>
#include <QHBoxLayout>
#include <QApplication>

using namespace std;

HelloVulkanApp::HelloVulkanApp()
{
	VulkanHelper::GetInstanceLayerExtensionProperties();

	m_hPipelineLayout = VK_NULL_HANDLE;
    m_hGraphicsPipeline = VK_NULL_HANDLE;
}

HelloVulkanApp::~HelloVulkanApp()
{
	vkDestroyPipeline(m_hDevice, m_hGraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_hDevice, m_hPipelineLayout, nullptr);

    // The below code is intentionally commentted in order to demonstrate Validation layers in action.
    // Expected output: The validation layer must complaint about the below memory leak
	// Destroy Vertex Buffer
	//vkDestroyBuffer(m_hDevice, VertexBuffer.m_Buffer, NULL);
	//vkFreeMemory(m_hDevice, VertexBuffer.m_Memory, NULL);
}

void HelloVulkanApp::Configure()
{
    SetApplicationName("Hello World!");
    SetWindowDimension(800, 600);

    // Add Validation Layers
    AddValidationLayer("VK_LAYER_LUNARG_standard_validation");

    // Add Vulkan instance extensions
    AddInstanceExtension(VK_KHR_SURFACE_EXTENSION_NAME);
    AddInstanceExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    AddInstanceExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
}

void HelloVulkanApp::Setup()
{
	CreateVertexBuffer(s_TriangleVertices, sizeof(s_TriangleVertices), sizeof(s_TriangleVertices[0]));
	
	CreateGraphicsPipeline();

	CreateCommandBuffers(); // Create command buffers

	RecordCommandBuffer();
}

void HelloVulkanApp::Update()
{
}

void HelloVulkanApp::CreateGraphicsPipeline()
{
    // Compile the vertex shader
	VkShaderModule vertShader = VulkanHelper::CreateShader(m_hDevice, "../source/shaders/TriangleVert.spv"); // Relative path to binary output dir

	// Setup the vertex shader stage create info structures
    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShader;
    vertShaderStageInfo.pName = "main";

    // Compile the fragment shader
	VkShaderModule fragShader = VulkanHelper::CreateShader(m_hDevice, "../source/shaders/TriangleFrag.spv"); // Relative path to binary output dir

	// Setup the fragment shader stage create info structures
    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShader;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    // Setup the vertex input
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = sizeof(m_VertexInputBinding) / sizeof(VkVertexInputBindingDescription);
	vertexInputInfo.pVertexBindingDescriptions = &m_VertexInputBinding;
	vertexInputInfo.vertexAttributeDescriptionCount = sizeof(m_VertexInputAttribute) / sizeof(VkVertexInputAttributeDescription);
	vertexInputInfo.pVertexAttributeDescriptions = m_VertexInputAttribute;


    // Setup input assembly
    // We will be rendering 1 triangle using triangle strip topology
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
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pushConstantRangeCount = 0;

	VkResult vkResult = vkCreatePipelineLayout(m_hDevice, &pipelineLayoutInfo, nullptr, &m_hPipelineLayout);

	if (vkResult != VK_SUCCESS)
	{
		VulkanHelper::LogError("vkCreatePipelineLayout() failed!");
		assert(false);
	}

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
	pipelineInfo.layout = m_hPipelineLayout;
	pipelineInfo.renderPass = m_hRenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	vkResult = vkCreateGraphicsPipelines(m_hDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_hGraphicsPipeline);
	if (vkResult != VK_SUCCESS)
	{
		VulkanHelper::LogError("vkCreateGraphicsPipelines() failed!");
		assert(false);
	}

	// Cleanup
	vkDestroyShaderModule(m_hDevice, fragShader, nullptr);
	vkDestroyShaderModule(m_hDevice, vertShader, nullptr);
}

void HelloVulkanApp::RecordCommandBuffer()
{
	// Background color (Blue)
	// The color values are defined in this order (Red,Green,Blue,Alpha)
	VkClearValue clearColor = { 0.0f, 0.0f, 1.0f, 1.0f };
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
		vkCmdBindPipeline(m_hCommandBufferList[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_hGraphicsPipeline);

		// Step 4: Specify vertex buffer information
		const VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(m_hCommandBufferList[i], 0, 1, &VertexBuffer.m_Buffer, offsets);
		
		// Step 5: Draw a quad using 3 vertices 
		vkCmdDraw(m_hCommandBufferList[i], 3, 1, 0, 0);

		// Step 6: End the Render pass
		vkCmdEndRenderPass(m_hCommandBufferList[i]);

		// Step 7: End the Command buffer
		VkResult vkResult = vkEndCommandBuffer(m_hCommandBufferList[i]);
		if (vkResult != VK_SUCCESS)
		{
			VulkanHelper::LogError("vkEndCommandBuffer() failed!");
			assert(false);
		}
	}
}

void HelloVulkanApp::CreateVertexBuffer(const void * vertexData, uint32_t dataSize, uint32_t dataStride)
{
	VulkanHelper::CreateBuffer(m_hDevice, m_physicalDeviceInfo.memProp,
		vertexData, dataSize, dataStride, &VertexBuffer.m_Buffer, &VertexBuffer.m_Memory);

	// Indicates the rate at which the information will be
	// injected for vertex input.
	m_VertexInputBinding.binding = 0;
	m_VertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	m_VertexInputBinding.stride = dataStride;

	// The VkVertexInputAttribute interpreting the data.
	m_VertexInputAttribute[0].binding = 0;
	m_VertexInputAttribute[0].location = 0;
	m_VertexInputAttribute[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	m_VertexInputAttribute[0].offset = offsetof(struct Vertex, m_Position);

	m_VertexInputAttribute[1].binding = 0;
	m_VertexInputAttribute[1].location = 1;
	m_VertexInputAttribute[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	m_VertexInputAttribute[1].offset = offsetof(struct Vertex, m_Color);
}

int main(int argc, char **argv)
{
	QApplication qtApp(argc, argv);

	VulkanApp* helloVulkanApp = new HelloVulkanApp(); // Create Vulkan app instance
	helloVulkanApp->EnableDepthBuffer(false);
	helloVulkanApp->Initialize();
	qtApp.exec();
	
	delete helloVulkanApp;
	return 0;
}