#include "Rect.h"

#include "../../../common/VulkanHelper.h"

#include <QMainWindow>
#include <QHBoxLayout>
#include <QApplication>
#include <random>

using namespace std;

#define VERTEX_BUFFER_BIND_IDX 0
#define INSTANCE_BUFFER_BIND_IDX 1
#define INSTANCE_COUNT 1000000
#define M_PI 3.14

Rect::Rect(VulkanApp* p_VulkanApp)
{
	m_hPipelineLayout = VK_NULL_HANDLE;
    m_hGraphicsPipeline = VK_NULL_HANDLE;

	memset(&UniformBuffer, 0, sizeof(UniformBuffer));
	memset(&m_VertexBuffer, 0, sizeof(m_VertexBuffer));
	memset(&m_InstanceBuffer, 0, sizeof(m_InstanceBuffer));
	
    m_VulkanApplication = p_VulkanApp;
}

Rect::~Rect()
{
    vkDestroyPipeline(m_VulkanApplication->m_hDevice, m_hGraphicsPipeline, nullptr);

	// Destroy Vertex Buffer
    vkDestroyBuffer(m_VulkanApplication->m_hDevice, m_VertexBuffer.m_Buffer, NULL);
    vkFreeMemory(m_VulkanApplication->m_hDevice, m_VertexBuffer.m_Memory, NULL);

	// Destroy descriptors
	for (int i = 0; i < descLayout.size(); i++) {
        vkDestroyDescriptorSetLayout(m_VulkanApplication->m_hDevice, descLayout[i], NULL);
	}
	descLayout.clear();

    vkDestroyPipelineLayout(m_VulkanApplication->m_hDevice, m_hPipelineLayout, nullptr);

    vkFreeDescriptorSets(m_VulkanApplication->m_hDevice, descriptorPool, (uint32_t)descriptorSet.size(), &descriptorSet[0]);
    vkDestroyDescriptorPool(m_VulkanApplication->m_hDevice, descriptorPool, NULL);

    vkUnmapMemory(m_VulkanApplication->m_hDevice, UniformBuffer.m_BufObj.m_Memory);
    vkDestroyBuffer(m_VulkanApplication->m_hDevice, UniformBuffer.m_BufObj.m_Buffer, NULL);
    vkFreeMemory(m_VulkanApplication->m_hDevice, UniformBuffer.m_BufObj.m_Memory, NULL);
}

void Rect::Setup()
{
	PrepareInstanceData();

    uint32_t dataSize = sizeof(rectVertices);
    uint32_t dataStride = sizeof(rectVertices[0]);
    CreateVertexBuffer(rectVertices, dataSize, dataStride);
	
	CreateDescriptor();

	CreateGraphicsPipeline();

	CreateCommandBuffers(); // Create command buffers

	RecordCommandBuffer();
}

void Rect::Update()
{
	glm::mat4 MVP = (*m_Projection) * (*m_View) * m_Model;

	VulkanHelper::WriteMemory(m_VulkanApplication->m_hDevice, 
								UniformBuffer.m_MappedMemory, 
								UniformBuffer.m_MappedRange,
								UniformBuffer.m_BufObj.m_MemoryFlags,
								&MVP, sizeof(MVP));
	//prepareInstanceData();
}

void Rect::ResizeWindow(int width, int height)
{
    // @Parminder: Should we show destroying and recreating pipeline or update 
    // the viewport and scissor dynamically (please check #if 0 code
    // in RecordCommandBuffers)
    // Pipeline creation could be an expensive call when multiple objects are in the scene
    VkDevice device = m_VulkanApplication->m_hDevice;
    vkDestroyPipeline(device, m_hGraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, m_hPipelineLayout, nullptr);
    CreateGraphicsPipeline();

    // Create command buffers and record commands
    CreateCommandBuffers();
    RecordCommandBuffer();
}

void Rect::CreateGraphicsPipeline()
{
    // Compile the vertex shader
    VkShaderModule vertShader = VulkanHelper::CreateShader(m_VulkanApplication->m_hDevice, "../source/shaders/RectInstanceVert.spv"); // Relative path to binary output dir

	// Setup the vertex shader stage create info structures
    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShader;
    vertShaderStageInfo.pName = "main";

    // Compile the fragment shader
    VkShaderModule fragShader = VulkanHelper::CreateShader(m_VulkanApplication->m_hDevice, "../source/shaders/RectInstanceFrag.spv"); // Relative path to binary output dir

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
	vertexInputInfo.pVertexBindingDescriptions = m_VertexInputBinding;
	vertexInputInfo.vertexAttributeDescriptionCount = sizeof(m_VertexInputAttribute) / sizeof(VkVertexInputAttributeDescription);
	vertexInputInfo.pVertexAttributeDescriptions = m_VertexInputAttribute;

    // Setup input assembly
    // We will be rendering 1 triangle using triangle strip topology
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

	// Setup viewport to the maximum widht and height of the window
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
    viewport.width = (float)m_VulkanApplication->m_swapChainExtent.width;
    viewport.height = (float)m_VulkanApplication->m_swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	// Setup scissor rect
	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
    scissor.extent = m_VulkanApplication->m_swapChainExtent;

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
	rasterizer.cullMode = VK_CULL_MODE_NONE/*VK_CULL_MODE_BACK_BIT*/;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthClampEnable = true;

	VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo = {};
	depthStencilStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilStateInfo.pNext = NULL;
	depthStencilStateInfo.flags = 0;
	depthStencilStateInfo.depthTestEnable = true;
	depthStencilStateInfo.depthWriteEnable = true;
	depthStencilStateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencilStateInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilStateInfo.stencilTestEnable = VK_FALSE;
	depthStencilStateInfo.back.failOp = VK_STENCIL_OP_KEEP;
	depthStencilStateInfo.back.passOp = VK_STENCIL_OP_KEEP;
	depthStencilStateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
	depthStencilStateInfo.back.compareMask = 0;
	depthStencilStateInfo.back.reference = 0;
	depthStencilStateInfo.back.depthFailOp = VK_STENCIL_OP_KEEP;
	depthStencilStateInfo.back.writeMask = 0;
	depthStencilStateInfo.minDepthBounds = 0;
	depthStencilStateInfo.maxDepthBounds = 0;
	depthStencilStateInfo.stencilTestEnable = VK_FALSE;
	depthStencilStateInfo.front = depthStencilStateInfo.back;

	// Setup multi sampling. In our first example we will be using single sampling mode
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = NUM_SAMPLES;

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
	pipelineLayoutInfo.setLayoutCount = (uint32_t)descLayout.size();
	pipelineLayoutInfo.pSetLayouts = descLayout.data();

    VkResult vkResult = vkCreatePipelineLayout(m_VulkanApplication->m_hDevice, &pipelineLayoutInfo, nullptr, &m_hPipelineLayout);

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
    pipelineInfo.renderPass = m_VulkanApplication->m_hRenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.pDepthStencilState = &depthStencilStateInfo;

    vkResult = vkCreateGraphicsPipelines(m_VulkanApplication->m_hDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_hGraphicsPipeline);
	if (vkResult != VK_SUCCESS)
	{
		VulkanHelper::LogError("vkCreateGraphicsPipelines() failed!");
		assert(false);
	}

	// Cleanup
    vkDestroyShaderModule(m_VulkanApplication->m_hDevice, fragShader, nullptr);
    vkDestroyShaderModule(m_VulkanApplication->m_hDevice, vertShader, nullptr);
}

void Rect::RecordCommandBuffer()
{
	// Specify the clear color value
	VkClearValue clearColor[2];
	clearColor[0].color.float32[0] = 0.0f;
	clearColor[0].color.float32[1] = 0.0f;
	clearColor[0].color.float32[2] = 0.0f;
	clearColor[0].color.float32[3] = 0.0f;

	// Specify the depth/stencil clear value
	clearColor[1].depthStencil.depth = 1.0f;
	clearColor[1].depthStencil.stencil = 0;

	// Offset to render in the frame buffer
	VkOffset2D   renderOffset = { 0, 0 };
	// Width & Height to render in the frame buffer
    VkExtent2D   renderExtent = m_VulkanApplication->m_swapChainExtent;

	// For each command buffers in the command buffer list
    for (size_t i = 0; i < m_VulkanApplication->m_hCommandBufferList.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		// Indicate that the command buffer can be resubmitted to the queue
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		// Begin command buffer
        vkBeginCommandBuffer(m_VulkanApplication->m_hCommandBufferList[i], &beginInfo);

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_VulkanApplication->m_hRenderPass;
        renderPassInfo.framebuffer = m_VulkanApplication->m_hFramebuffers[i];
		renderPassInfo.renderArea.offset = renderOffset;
		renderPassInfo.renderArea.extent = renderExtent;
		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearColor;

		// Begin render pass
        vkCmdBeginRenderPass(m_VulkanApplication->m_hCommandBufferList[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Bind graphics pipeline
        vkCmdBindPipeline(m_VulkanApplication->m_hCommandBufferList[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_hGraphicsPipeline);

        vkCmdBindDescriptorSets(m_VulkanApplication->m_hCommandBufferList[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_hPipelineLayout,
			0, 1, descriptorSet.data(), 0, NULL);

		// Specify vertex buffer information
		const VkDeviceSize offsets[1] = { 0 };
        vkCmdBindVertexBuffers(m_VulkanApplication->m_hCommandBufferList[i], VERTEX_BUFFER_BIND_IDX, 1, &m_VertexBuffer.m_Buffer, offsets);
		vkCmdBindVertexBuffers(m_VulkanApplication->m_hCommandBufferList[i], INSTANCE_BUFFER_BIND_IDX, 1, &m_InstanceBuffer.m_Buffer, offsets);

		// Draw the Cube 
		const int vertexCount = sizeof(rectVertices) / sizeof(Vertex);
		vkCmdDraw(m_VulkanApplication->m_hCommandBufferList[i], vertexCount, INSTANCE_COUNT, 0, 0);

		// End the Render pass
        vkCmdEndRenderPass(m_VulkanApplication->m_hCommandBufferList[i]);

		// End the Command buffer
        VkResult vkResult = vkEndCommandBuffer(m_VulkanApplication->m_hCommandBufferList[i]);
		if (vkResult != VK_SUCCESS)
		{
			VulkanHelper::LogError("vkEndCommandBuffer() failed!");
			assert(false);
		}
	}
}

void Rect::CreateVertexBuffer(const void * vertexData, uint32_t dataSize, uint32_t dataStride)
{
	m_VertexBuffer.m_DataSize = dataSize;
	m_VertexBuffer.m_MemoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

	const VkPhysicalDeviceMemoryProperties& memProp = m_VulkanApplication->m_physicalDeviceInfo.memProp;
	const VkDevice& device = m_VulkanApplication->m_hDevice;
	VulkanHelper::CreateBuffer(device, memProp, m_VertexBuffer, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexData);

	// Indicates the rate at which the information will be
	// injected for vertex input.
	m_VertexInputBinding[0].binding = VERTEX_BUFFER_BIND_IDX;
	m_VertexInputBinding[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	m_VertexInputBinding[0].stride = dataStride;

	m_VertexInputBinding[1].binding = INSTANCE_BUFFER_BIND_IDX;
	m_VertexInputBinding[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
	m_VertexInputBinding[1].stride = sizeof(InstanceData);

	// The VkVertexInputAttribute interpreting the data.
	m_VertexInputAttribute[0].binding = VERTEX_BUFFER_BIND_IDX;
	m_VertexInputAttribute[0].location = 0;
	m_VertexInputAttribute[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	m_VertexInputAttribute[0].offset = offsetof(Vertex, m_Position);

	m_VertexInputAttribute[1].binding = VERTEX_BUFFER_BIND_IDX;
	m_VertexInputAttribute[1].location = 1;
	m_VertexInputAttribute[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	m_VertexInputAttribute[1].offset = offsetof(Vertex, m_Color);
	////////////////////////////////////////////////////////////////////////////////////

	m_VertexInputAttribute[2].binding = INSTANCE_BUFFER_BIND_IDX;
	m_VertexInputAttribute[2].location = 2;
	m_VertexInputAttribute[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	m_VertexInputAttribute[2].offset = 0;

	m_VertexInputAttribute[3].binding = INSTANCE_BUFFER_BIND_IDX;
	m_VertexInputAttribute[3].location = 3;
	m_VertexInputAttribute[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	m_VertexInputAttribute[3].offset = 16 * 1;

	m_VertexInputAttribute[4].binding = INSTANCE_BUFFER_BIND_IDX;
	m_VertexInputAttribute[4].location = 4;
	m_VertexInputAttribute[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	m_VertexInputAttribute[4].offset = 16 * 2;
	
	m_VertexInputAttribute[5].binding = INSTANCE_BUFFER_BIND_IDX;
	m_VertexInputAttribute[5].location = 5;
	m_VertexInputAttribute[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	m_VertexInputAttribute[5].offset = 16 * 3;	
}

void Rect::CreateCommandBuffers()
{
    m_VulkanApplication->CreateCommandBuffers();
}

void Rect::CreateUniformBuffer()
{
	UniformBuffer.m_BufObj.m_MemoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	UniformBuffer.m_BufObj.m_DataSize = sizeof(glm::mat4);

	// Create buffer resource states using VkBufferCreateInfo
	VulkanHelper::CreateBuffer(m_VulkanApplication->m_hDevice, m_VulkanApplication->m_physicalDeviceInfo.memProp, UniformBuffer.m_BufObj, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

	// Map the GPU memory on to local host
	VulkanHelper::MapMemory(m_VulkanApplication->m_hDevice, UniformBuffer.m_BufObj.m_Memory, 0, UniformBuffer.m_BufObj.m_MemRqrmnt.size, 0, UniformBuffer.m_MappedMemory);

	// We have only one Uniform buffer object to update
    UniformBuffer.m_MappedRange.resize(1);

	// Populate the VkMappedMemoryRange data structure
    UniformBuffer.m_MappedRange[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    UniformBuffer.m_MappedRange[0].memory = UniformBuffer.m_BufObj.m_Memory;
    UniformBuffer.m_MappedRange[0].offset = 0;
    UniformBuffer.m_MappedRange[0].size = UniformBuffer.m_BufObj.m_DataSize;

	// Update descriptor buffer info in order to write the descriptors
    UniformBuffer.m_DescriptorBufInfo.buffer = UniformBuffer.m_BufObj.m_Buffer;
    UniformBuffer.m_DescriptorBufInfo.offset = 0;
    UniformBuffer.m_DescriptorBufInfo.range = UniformBuffer.m_BufObj.m_DataSize;
}

void Rect::DestroyUniformBuffer()
{
    vkUnmapMemory(m_VulkanApplication->m_hDevice, UniformBuffer.m_BufObj.m_Memory);
    vkDestroyBuffer(m_VulkanApplication->m_hDevice, UniformBuffer.m_BufObj.m_Buffer, NULL);
    vkFreeMemory(m_VulkanApplication->m_hDevice, UniformBuffer.m_BufObj.m_Memory, NULL);
}

void Rect::CreateDescriptorSetLayout()
{
	// Define the layout binding information for the descriptor set(before creating it)
	// Specify binding point, shader type(like vertex shader below), count etc.
	VkDescriptorSetLayoutBinding layoutBindings[1];
	layoutBindings[0].binding = 0; // DESCRIPTOR_SET_BINDING_INDEX
	layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBindings[0].descriptorCount = 1;
	layoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	layoutBindings[0].pImmutableSamplers = NULL;

	// Specify the layout bind into the VkDescriptorSetLayoutCreateInfo
	// and use it to create a descriptor set layout
	VkDescriptorSetLayoutCreateInfo descriptorLayout = {};
	descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorLayout.pNext = NULL;
	descriptorLayout.bindingCount = 1;
	descriptorLayout.pBindings = layoutBindings;

	VkResult  result;
	// Allocate required number of descriptor layout objects and  
	// create them using vkCreateDescriptorSetLayout()
	descLayout.resize(1);
    result = vkCreateDescriptorSetLayout(m_VulkanApplication->m_hDevice, &descriptorLayout, NULL, descLayout.data());
	assert(result == VK_SUCCESS);
}

void Rect::DestroyDescriptorLayout()
{
	for (int i = 0; i < descLayout.size(); i++) {
        vkDestroyDescriptorSetLayout(m_VulkanApplication->m_hDevice, descLayout[i], NULL);
	}
	descLayout.clear();
}

void Rect::CreateDescriptor()
{
	CreateDescriptorSetLayout();
	CreateUniformBuffer();
	CreateDescriptorPool();
	CreateDescriptorSet();
}

void Rect::CreateDescriptorPool()
{
	VkResult  result;
	// Define the size of descriptor pool based on the
	// type of descriptor set being used.
	std::vector<VkDescriptorPoolSize> descriptorTypePool;

	// The first descriptor pool object is of type Uniform buffer
	descriptorTypePool.push_back(VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 });

	// Populate the descriptor pool state information
	// in the create info structure.
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.pNext = NULL;
	descriptorPoolCreateInfo.maxSets = 1;
	descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	descriptorPoolCreateInfo.poolSizeCount = (uint32_t)descriptorTypePool.size();
	descriptorPoolCreateInfo.pPoolSizes = descriptorTypePool.data();

	// Create the descriptor pool using the descriptor 
	// pool create info structure
    result = vkCreateDescriptorPool(m_VulkanApplication->m_hDevice, &descriptorPoolCreateInfo, NULL, &descriptorPool);
	assert(result == VK_SUCCESS);
}

void Rect::CreateDescriptorSet()
{
	VkResult  result;

	// Create the descriptor allocation structure and specify the descriptor 
	// pool and descriptor layout
	VkDescriptorSetAllocateInfo dsAllocInfo[1];
	dsAllocInfo[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	dsAllocInfo[0].pNext = NULL;
	dsAllocInfo[0].descriptorPool = descriptorPool;
	dsAllocInfo[0].descriptorSetCount = 1;
	dsAllocInfo[0].pSetLayouts = descLayout.data();

	// Allocate the number of descriptor sets needs to be produced
	descriptorSet.resize(1);

	// Allocate descriptor sets
    result = vkAllocateDescriptorSets(m_VulkanApplication->m_hDevice, dsAllocInfo, descriptorSet.data());
	assert(result == VK_SUCCESS);

	// Allocate one write descriptors for transformation (MVP)
	VkWriteDescriptorSet writes[1];
	memset(&writes, 0, sizeof(writes));

	// Specify the uniform buffer related 
	// information into first write descriptor
	writes[0] = {};
	writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writes[0].pNext = NULL;
	writes[0].dstSet = descriptorSet[0];
	writes[0].descriptorCount = 1;
	writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &UniformBuffer.m_DescriptorBufInfo;
	writes[0].dstArrayElement = 0;
	writes[0].dstBinding = 0; // DESCRIPTOR_SET_BINDING_INDEX

	// Update the uniform buffer into the allocated descriptor set
    vkUpdateDescriptorSets(m_VulkanApplication->m_hDevice, 1, writes, 0, NULL);
}

void Rect::PrepareInstanceData()
{
	std::vector<InstanceData> instanceData;
	instanceData.resize(INSTANCE_COUNT);

	std::mt19937 rndGenerator(time(NULL));
	std::uniform_real_distribution<double> uniformDist(0.0, 1.0);

	for (auto i = 0; i < INSTANCE_COUNT; i++)
	{
		const float theta = 2 * M_PI * uniformDist(rndGenerator);
		const float phi = acos(1 - 2 * uniformDist(rndGenerator));
		glm::vec3 pos = glm::vec3(30 * sin(phi) * cos(theta), 
   								  40 * sin(theta) , 
								  cos(phi)) * 20.0f;

		instanceData[i].MVP = glm::translate(glm::mat4(1.0f), pos);
	}

	VkMemoryPropertyFlags memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	m_InstanceBuffer.m_MemoryFlags = memoryProperty;
	m_InstanceBuffer.m_DataSize = instanceData.size() * sizeof(InstanceData);

	VulkanHelper::CreateStagingBuffer(m_VulkanApplication->m_hDevice,
		m_VulkanApplication->m_physicalDeviceInfo.memProp,
		m_VulkanApplication->m_hCommandPool,
		m_VulkanApplication->m_hGraphicsQueue,
		m_InstanceBuffer,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		instanceData.data());
}
