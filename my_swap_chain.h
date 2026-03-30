#ifndef __MY_SWAP_CHAIN_H__
#define __MY_SWAP_CHAIN_H__

#include "my_device.h"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <memory>
#include <string>
#include <vector>

class MySwapChain 
{
public:
    // Please note that the value of MAX_FRAMES_IN_FLIGHT depends on the capability of the graphics card
    static int MAX_FRAMES_IN_FLIGHT;

    MySwapChain(MyDevice &deviceRef, VkExtent2D windowExtent);
    MySwapChain(MyDevice& deviceRef, VkExtent2D windowExtent, std::shared_ptr<MySwapChain> previous);

    ~MySwapChain();

    MySwapChain(const MySwapChain &) = delete;
    MySwapChain& operator=(const MySwapChain &) = delete;
	MySwapChain(MySwapChain&&) = delete;
	MySwapChain& operator=(const MySwapChain&&) = delete;

    VkFramebuffer frameBuffer(int index) { return m_vVkSwapChainFramebuffers[index]; }
    VkRenderPass  renderPass()           { return m_vkRenderPass; }
    VkImageView   imageView(int index)   { return m_vVkSwapChainImageViews[index]; }
    size_t        imageCount()           { return m_vVkSwapChainImages.size(); }
    VkFormat      swapChainImageFormat() { return m_vkSwapChainImageFormat; }
    VkExtent2D    swapChainExtent()      { return m_vkSwapChainExtent; }
    uint32_t      width()                { return m_vkSwapChainExtent.width; }
    uint32_t      height()               { return m_vkSwapChainExtent.height; }

    float extentAspectRatio() 
    {
      return static_cast<float>(m_vkSwapChainExtent.width) / static_cast<float>(m_vkSwapChainExtent.height);
    }

    VkFormat findDepthFormat();

    VkResult acquireNextImage(uint32_t *imageIndex);
    VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

    bool compareSwapFormats(const MySwapChain& swapChain) const
    {
        return (swapChain.m_vkSwapChainDepthFormat == m_vkSwapChainDepthFormat &&
            swapChain.m_vkSwapChainImageFormat == m_vkSwapChainImageFormat);
    }

private:
    void _init();
    void _createSwapChain();
    void _createImageViews();
    void _createDepthResources();
    void _createRenderPass();
    void _createFramebuffers();
    void _createSyncObjects();

    // Helper functions
    VkSurfaceFormatKHR _chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR   _chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D         _chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    VkFormat                     m_vkSwapChainImageFormat;
    VkFormat                     m_vkSwapChainDepthFormat;
    VkExtent2D                   m_vkSwapChainExtent;

    std::vector<VkFramebuffer>   m_vVkSwapChainFramebuffers;
    VkRenderPass                 m_vkRenderPass;

    // Depth
    std::vector<VkImage>         m_vVkDepthImages;
    std::vector<VkDeviceMemory>  m_vVkDepthImageMemorys;
    std::vector<VkImageView>     m_vVkDepthImageViews;

    // Swapchain
    std::vector<VkImage>         m_vVkSwapChainImages;
    std::vector<VkImageView>     m_vVkSwapChainImageViews;

    MyDevice                    &m_myDevice;
    VkExtent2D                   m_vkWindowExtent;

    VkSwapchainKHR               m_vkSwapChain;
    std::shared_ptr<MySwapChain> m_pMyOldSwapChain;

    std::vector<VkSemaphore>     m_vVkImageAvailableSemaphores;
    std::vector<VkSemaphore>     m_vVkRenderFinishedSemaphores;
    std::vector<VkFence>         m_vVkInFlightFences;
    size_t                       m_iCurrentFrame = 0;
};

#endif

