#pragma once
#include "VulkanHelper.h"

#include <memory>

#include <QWindow>
#include <QTimer>
#include <QElapsedtimer>
#include <QApplication>

/*********** GLM HEADER FILES ***********/
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT

class VulkanApp;
class Window;
class DrawableInterface;

class AbstractApp
{
public:
	AbstractApp() {}
	virtual ~AbstractApp() {}
};

// Base class for Vulkan application
class VulkanApp : public AbstractApp
{
public:
    VulkanApp();
	virtual ~VulkanApp();

    void Initialize(); // Initialize the Vulkan application
    void Run();  // Render loop

    void SetApplicationName(string appName) { m_appName = appName; }
    string GetAppllicationName() { return (m_appName); }
	void SetWindowDimension(int width, int height);
    VkExtent2D GetWindowDimension() { return (m_windowDim); }
	void EnableDepthBuffer(bool depthEnabled) { m_DepthEnabled = depthEnabled; }
    void EnableWindowResize(bool resizeEnabled) { m_ReSizeEnabled = resizeEnabled; }
    
    void AddValidationLayer(char* pName) { m_validationLayers.push_back(pName);  }
    void AddInstanceExtension(char* pName) { m_instanceExtensionNames.push_back(pName); }

	virtual void CreateCommandBuffers(); // Overide the default implementation as per application requirement

    virtual void ResizeWindow(int width, int height);
	virtual void mousePressEvent(QMouseEvent* p_Event) {} // Default implementation, Overide as required
	virtual void mouseReleaseEvent(QMouseEvent* p_Event) {}
	virtual void mouseMoveEvent(QMouseEvent* p_Event) {}

    void SetView(void *pView) {
      m_pView = pView;
    }

    void* GetWinID(WId p_WinID);
protected:
	// Core virtual methods used by derived classes
	virtual void Configure() = 0; // Application's user configuration prior to Setup()
	virtual void Setup() = 0;     // Set's up the drawing pipeline
	virtual void Update() = 0;    // Update data prior to Render() & Present() such as updating locals, uniforms etc.

    virtual bool Render();		  // Draw the primitive on surface
    virtual bool Present();		  // Swap the drawn surface on application window

private:
	// Initialization functions for Vulkan application
	void InitializeVulkan();
	void CreateVulkanInstance();
	void CreateSurface();
	
	// Device creation objects
	void CreateVulkanDeviceAndQueue();
	    // Helper functions for CreateVulkanDeviceAndQueue()
		void SelectPhysicalDevice();
		void GetPhysicalDeviceInfo(VkPhysicalDevice device, PhysicalDeviceInfo* pDeviceInfo);
		bool IsDeviceSuitable(PhysicalDeviceInfo deviceInfo);
	void CreateDeviceAndQueueObjects();

	void CreateSwapChain();
	void CreateDepthImage();

	void CreateSemaphores();

	virtual void CreateRenderPass();
	void CreateFramebuffers();

public:
	struct {
		VkFormat		m_Format;
		VulkanImageView m_ImageView;
		VulkanImage		m_Image;
	}DepthImage;
	VkCommandBuffer		cmdBufferDepthImage;			// Command buffer for depth image layout

	// Application display window
    Window*		    m_pWindow;		// Display window object
	VkExtent2D		m_windowDim;	// Display window dimension
	string          m_appName;		// Display name
	bool			m_DepthEnabled; // Is depth buffer supported
    bool            m_ReSizeEnabled;// Is Window resize support enabled

    std::vector<const char *> m_instanceExtensionNames;
    std::vector<const char *> m_validationLayers;

    // Vulkan specific objects
    VkInstance      m_hInstance; // Vulkan instance object    
	VkSurfaceKHR    m_hSurface;  // Vulkan presentation surface

    // Vulkan Device specific objects
    VkPhysicalDevice            m_hPhysicalDevice;
    PhysicalDeviceInfo          m_physicalDeviceInfo;
    VkDevice                    m_hDevice;
    std::vector<const char*>    m_requiredDeviceExtensionList;

    // Pointers to Graphics & Present queue
    VkQueue                     m_hGraphicsQueue;
    VkQueue                     m_hPresentQueue;

    // Swap chain specific objects
	VkSwapchainKHR              m_hSwapChain;
    VkFormat                    m_hSwapChainImageFormat;
    VkExtent2D                  m_swapChainExtent;
    std::vector<VkImageView>    m_hSwapChainImageViewList;
    uint32_t                    m_activeSwapChainImageIndex;

    // Render Pass
    VkRenderPass                m_hRenderPass;
	// Frame Buffer
	std::vector<VkFramebuffer>  m_hFramebuffers;

    // Command buffer related objects
    VkCommandPool                   m_hCommandPool;
    std::vector<VkCommandBuffer>    m_hCommandBufferList;
	
	// Presentation synchronization objects
	VkSemaphore                     m_hRenderReadySemaphore;
    VkSemaphore                     m_hPresentReadySemaphore;
    void* m_pView;
    QElapsedTimer FPS; int m_Frame;
};

class Window : public QWindow
{
	Q_OBJECT

public:
	Window(VulkanApp* parent = NULL);
	~Window() { delete renderTimer; }

	public slots:
	void Run();
    void resizeEvent(QResizeEvent* pEvent);
	virtual void mousePressEvent(QMouseEvent* p_Event) { m_VulkanApp->mousePressEvent(p_Event); }
	virtual void mouseReleaseEvent(QMouseEvent* p_Event) { m_VulkanApp->mouseReleaseEvent(p_Event); }
	virtual void mouseMoveEvent(QMouseEvent* p_Event) { m_VulkanApp->mouseMoveEvent(p_Event); }

private:
	QTimer* renderTimer;	// Refresh timer
	VulkanApp* m_VulkanApp; // Used to call run() by the timer
};

class DrawableInterface
{
public:
	// Life Cycle
	virtual void Setup() UNIMPLEMENTED_INTEFACE
	virtual void Update() UNIMPLEMENTED_INTEFACE

	// Transformation
	void Rotate(float p_Angle, float p_X, float p_Y, float p_Z) { m_Model = glm::rotate(glm::mat4(), p_Angle, glm::vec3(p_X, p_Y, p_Z)); }
	void Translate(float p_X, float p_Y, float p_Z) { m_Model = glm::translate(glm::mat4(), glm::vec3(p_X, p_Y, p_Z)); }
	void Scale(float p_X, float p_Y, float p_Z) { m_Model = glm::translate(glm::mat4(), glm::vec3(p_X, p_Y, p_Z)); }
	void Reset() { m_Model = glm::mat4(); }
	GETSET(glm::mat4, Model)		// Owned by drawable item
	GETSET(glm::mat4*, Projection)	// Not owned by drawable item  
	GETSET(glm::mat4*, View)		// Not owned by drawable item

	// Mouse interaction: Dummy interface for now.
	virtual void mousePressEvent() UNIMPLEMENTED_INTEFACE
	virtual void mouseReleaseEvent() UNIMPLEMENTED_INTEFACE
	virtual void mouseMoveEvent() UNIMPLEMENTED_INTEFACE
	virtual void mouseDoubleClickEvent() UNIMPLEMENTED_INTEFACE

	// Key interaction: Dummy interface for now.
	virtual void keyPressEvent() UNIMPLEMENTED_INTEFACE

    // Application Window resizing
    virtual void ResizeWindow(int width, int height) {}
};
