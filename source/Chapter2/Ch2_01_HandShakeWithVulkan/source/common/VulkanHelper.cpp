#include "VulkanHelper.h"

VulkanHelper::VulkanHelper()
{
}

VulkanHelper::~VulkanHelper()
{
}

VkResult VulkanHelper::GetValidationLayers()
{
	uint32_t						instanceLayerCount;		// Stores number of layers supported by instance
	std::vector<VkLayerProperties>	layerProperties;		// Vector to store layer properties

	// Query all the layers
	VkResult result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, NULL);
	layerProperties.resize(instanceLayerCount);
	result = vkEnumerateInstanceLayerProperties(&instanceLayerCount, layerProperties.data());
	
	if (result) return result;

	// Query all the extensions for each layer and store it.
	std::cout << "\n  ********Instance Layers ********" << std::endl;
	std::cout << "-----------------------------------" << std::endl;
	for (auto globalLayerProp : layerProperties)
		std::cout << "\n" << globalLayerProp.description << "\n\t|\n\t|---[Layer Name]--> " << globalLayerProp.layerName << "\n";
}