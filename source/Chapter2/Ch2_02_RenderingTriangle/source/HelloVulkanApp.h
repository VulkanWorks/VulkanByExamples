#pragma once
#include "../../../common/VulkanApp.h"

class HelloVulkanApp : public VulkanApp
{
public:
    HelloVulkanApp();
	virtual ~HelloVulkanApp();

    //void SetApplicationName(string name);

protected:
	virtual void Configure();
	virtual void Setup();
	virtual void Update();

private:
	void CreateGraphicsPipeline();
	void RecordCommandBuffer();

    // Store app specific objects
    VkPipelineLayout m_hPipelineLayout;
    VkPipeline       m_hGraphicsPipeline;
};