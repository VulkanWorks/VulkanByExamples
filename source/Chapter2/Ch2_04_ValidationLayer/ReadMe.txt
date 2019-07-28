This program intentionally leave a memory leak behind to demonstrate the Validation layer memory leak detection in Vulkan.
In order to fix this memory leak uncomment for following code in the HelloVulkanApp class's destructor.

	// Destroy Vertex Buffer
	//vkDestroyBuffer(m_hDevice, VertexBuffer.buffer, NULL);
	//vkFreeMemory(m_hDevice, VertexBuffer.memory, NULL);


HelloVulkanApp::~HelloVulkanApp()
{
	vkDestroyPipeline(m_hDevice, m_hGraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_hDevice, m_hPipelineLayout, nullptr);

	// Destroy Vertex Buffer
	vkDestroyBuffer(m_hDevice, VertexBuffer.buffer, NULL);
	vkFreeMemory(m_hDevice, VertexBuffer.memory, NULL);
}
