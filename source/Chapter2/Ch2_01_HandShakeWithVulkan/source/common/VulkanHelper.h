#pragma once
#include <vulkan/vulkan.h>

#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <set>
#include <vector>
using namespace std;

// A structure to store physical device information
struct PhysicalDeviceInfo
{
    VkPhysicalDeviceProperties              prop = {};
    VkPhysicalDeviceFeatures                features = {};
    VkPhysicalDeviceMemoryProperties        memProp = {};
    std::vector<VkQueueFamilyProperties>    familyPropList;
    int                                     graphicsFamilyIndex = -1;
    int                                     presentFamilyIndex = -1;
    std::vector<VkExtensionProperties>      extensionList;
    VkSurfaceCapabilitiesKHR                capabilities = {};
    std::vector<VkSurfaceFormatKHR>         formatList;
    std::vector<VkPresentModeKHR>           presentModeList;
};

struct LayerProperties
{
	VkLayerProperties						properties;
	std::vector<VkExtensionProperties>		extensions;
};

class VulkanHelper
{
public:
	VulkanHelper();
	virtual ~VulkanHelper();

	// Layer and Extension helper functions
	static VkResult GetValidationLayers();

private:
	// Layer property list containing Layers and respective extensions
	std::vector<LayerProperties> m_LayerPropertyList;
};