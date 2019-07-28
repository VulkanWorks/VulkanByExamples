#include "HelloVulkanApp.h"
using namespace std;

HelloVulkanApp::HelloVulkanApp()
{
	VulkanHelper::GetValidationLayers();
}

HelloVulkanApp::~HelloVulkanApp()
{
}

int main(int argc, char **argv)
{
	VulkanApp* helloVulkanApp = new HelloVulkanApp(); // Create Vulkan app instance
	helloVulkanApp->Initialize();
	delete helloVulkanApp;
	return 0;
}