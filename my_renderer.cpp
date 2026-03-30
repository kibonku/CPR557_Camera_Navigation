#include "my_renderer.h"

// std
#include <array>
#include <cassert>
#include <stdexcept>

MyRenderer::MyRenderer(MyWindow& window, MyDevice& device)
    : m_myWindow{ window },
	  m_myDevice{ device }
{
    m_iCurrentImageIndex = 0;
    m_iCurrentFrameIndex = 0;
    m_bIsFrameStarted = false;

    _recreateSwapChain();
    _createCommandBuffers();
}

MyRenderer::~MyRenderer() 
{ 
    _freeCommandBuffers();
}

void MyRenderer::_recreateSwapChain()
{
    // make sure the window's size is not 0
    auto extent = m_myWindow.extent();
    while (extent.width == 0 || extent.height == 0)
    {
        extent = m_myWindow.extent();
        m_myWindow.waitEvents();
    }

    vkDeviceWaitIdle(m_myDevice.device());

    if (m_mySwapChain == nullptr)
    {
        m_mySwapChain = std::make_unique<MySwapChain>(m_myDevice, extent);
    }
    else
    {
        std::shared_ptr<MySwapChain> oldSwapChain = std::move(m_mySwapChain);
        m_mySwapChain = std::make_unique<MySwapChain>(m_myDevice, extent, oldSwapChain);

        if (!oldSwapChain->compareSwapFormats(*m_mySwapChain.get()))
        {
            throw std::runtime_error("Swap chain image(or depth) format has changed!");
        }
    }
}

void MyRenderer::_createCommandBuffers()
{
    m_vVkCommandBuffers.resize(MySwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_myDevice.commandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_vVkCommandBuffers.size());

    if (vkAllocateCommandBuffers(m_myDevice.device(), &allocInfo, m_vVkCommandBuffers.data()) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void MyRenderer::_freeCommandBuffers()
{
    vkFreeCommandBuffers(
        m_myDevice.device(),
        m_myDevice.commandPool(),
        static_cast<uint32_t>(m_vVkCommandBuffers.size()),
        m_vVkCommandBuffers.data());

    m_vVkCommandBuffers.clear();
}

VkCommandBuffer MyRenderer::beginFrame()
{
    assert(!m_bIsFrameStarted && "Can't call beginFrame while already in progress");

    auto result = m_mySwapChain->acquireNextImage(&m_iCurrentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        _recreateSwapChain();
        return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    m_bIsFrameStarted = true;

    auto commandBuffer = _currentCommandBuffer();
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    return commandBuffer;
}

void MyRenderer::endFrame()
{
    assert(m_bIsFrameStarted && "Can't call endFrame while frame is not in progress");
    auto commandBuffer = _currentCommandBuffer();
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }

    auto result = m_mySwapChain->submitCommandBuffers(&commandBuffer, &m_iCurrentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_myWindow.wasWindowResized())
    {
        m_myWindow.resetWindowResizedFlag();
        _recreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    m_bIsFrameStarted = false;
    m_iCurrentFrameIndex = (m_iCurrentFrameIndex + 1) % MySwapChain::MAX_FRAMES_IN_FLIGHT;
}

void MyRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
    assert(m_bIsFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(
        commandBuffer == _currentCommandBuffer() &&
        "Can't begin render pass on command buffer from a different frame");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_mySwapChain->renderPass();
    renderPassInfo.framebuffer = m_mySwapChain->frameBuffer(m_iCurrentImageIndex);

    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = m_mySwapChain->swapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_mySwapChain->swapChainExtent().width);
    viewport.height = static_cast<float>(m_mySwapChain->swapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{ {0, 0}, m_mySwapChain->swapChainExtent() };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void MyRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) 
{
    assert(m_bIsFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
    assert(
        commandBuffer == _currentCommandBuffer() &&
        "Can't end render pass on command buffer from a different frame");
    vkCmdEndRenderPass(commandBuffer);
}

