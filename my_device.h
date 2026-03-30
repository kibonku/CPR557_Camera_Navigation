#ifndef __MY_DEVICE_H__
#define __MY_DEVICE_H__

#include "my_window.h"

// std lib headers
#include <string>
#include <vector>

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentModes;
};

struct QueueFamilyIndices
{
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    bool     graphicsFamilyHasValue = false;
    bool     presentFamilyHasValue = false;
    bool     isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
};

class MyDevice 
{
  public:
#ifdef NDEBUG
    const bool m_bEnableValidationLayers = false;
#else
    const bool m_bEnableValidationLayers = true;
#endif

    MyDevice(MyWindow &window);
    ~MyDevice();

    // Not copyable or movable
    MyDevice(const MyDevice &) = delete;
    MyDevice& operator=(const MyDevice &) = delete;
    MyDevice(MyDevice &&) = delete;
    MyDevice &operator=(MyDevice &&) = delete;

    VkCommandPool commandPool() { return m_vkCommandPool; }
    VkDevice device()           { return m_vkDevice; }
    VkSurfaceKHR surface()      { return m_vkSurface; }
    VkQueue graphicsQueue()     { return m_vkGraphicsQueue; }
    VkQueue presentQueue()      { return m_vkPresentQueue; }

    // Used by Swap Chain
    SwapChainSupportDetails getSwapChainSupport()  { return _querySwapChainSupport(m_vkPhysicalDevice); }
    QueueFamilyIndices findPhysicalQueueFamilies() { return _findQueueFamilies(m_vkPhysicalDevice); }
    VkFormat findSupportedFormat(
        const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    void createImageWithInfo(
        const VkImageCreateInfo &imageInfo,
        VkMemoryPropertyFlags properties,
        VkImage &image,
        VkDeviceMemory &imageMemory);

    // Buffer Helper Functions
    void createBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer &buffer,
        VkDeviceMemory &bufferMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

  private:
    void _createInstance();
    void _setupDebugMessenger();
    void _pickPhysicalDevice();
    void _createSurface();
    void _createLogicalDevice();
    void _createCommandPool();
    uint32_t _findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    // helper functions
    bool _isDeviceSuitable(VkPhysicalDevice device);
    std::vector<const char *> _getRequiredExtensions();
    bool _checkValidationLayerSupport();
    QueueFamilyIndices _findQueueFamilies(VkPhysicalDevice device);
    void _populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    void _hasGflwRequiredInstanceExtensions();
    bool _checkDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails _querySwapChainSupport(VkPhysicalDevice device);

    VkInstance                 m_vkInstance;
    VkDebugUtilsMessengerEXT   m_vkDebugMessenger;
    VkPhysicalDevice           m_vkPhysicalDevice = VK_NULL_HANDLE;

    MyWindow                  &m_myWindow;
    VkCommandPool              m_vkCommandPool;

    VkDevice                   m_vkDevice;
    VkSurfaceKHR               m_vkSurface;
    VkQueue                    m_vkGraphicsQueue;
    VkQueue                    m_vkPresentQueue;

    const std::vector<const char *> validationLayers = { "VK_LAYER_KHRONOS_validation" };
    const std::vector<const char *> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
};

#endif

