#include "my_application.h"

// Render factory
#include "my_simple_render_factory.h"
#include "my_keyboard_controller.h"

// use radian rather degree for angle
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// Std
#include <stdexcept>
#include <array>
#include <chrono>
#include <iostream>

MyApplication::MyApplication() :
    m_bPerspectiveProjection(true)
{
    _loadGameObjects();
}

void MyApplication::run() 
{
    static bool bPerspectiveCamera = true;

    m_myWindow.bindMyApplication(this);
    MySimpleRenderFactory simpleRenderFactory{ m_myDevice, m_myRenderer.swapChainRenderPass() };

    auto viewerObject = MyGameObject::createGameObject();

    auto currentTime = std::chrono::high_resolution_clock::now();

    while (!m_myWindow.shouldClose()) 
    {
        m_myWindow.pollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        float apsectRatio = m_myRenderer.aspectRatio();

        if (m_bPerspectiveProjection)
            m_myCamera.setPerspectiveProjection(glm::radians(50.f), apsectRatio, 0.1f, 100.f);
        else
            m_myCamera.setOrthographicProjection(-apsectRatio * 3.0f, apsectRatio * 3.0f, -3.0f, 3.0f, -50.0f, 50.0f);

        if (auto commandBuffer = m_myRenderer.beginFrame())
        {
            m_myRenderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderFactory.renderGameObjects(commandBuffer, m_vMyGameObjects, m_myCamera);
            m_myRenderer.endSwapChainRenderPass(commandBuffer);
            m_myRenderer.endFrame();
        }
    }

    vkDeviceWaitIdle(m_myDevice.device());
}

void MyApplication::switchProjectionMatrix()
{
    m_bPerspectiveProjection = !m_bPerspectiveProjection;
}

void MyApplication::_loadGameObjects()
{
    glm::vec3 sceneMin( 1e30f,  1e30f,  1e30f);
    glm::vec3 sceneMax(-1e30f, -1e30f, -1e30f);

    // Object 1: teapot at origin
    glm::vec3 min, max;
    auto model1 = MyModel::createModelFromFile(m_myDevice, "models/teapot.obj", min, max);
    std::cout << "Teapot bbox X: " << min.x << " to " << max.x << std::endl;  
    auto teapot1 = MyGameObject::createGameObject();
    teapot1.model = std::move(model1);
    teapot1.transform.translation = glm::vec3{0.0f, 0.0f, 0.0f};
    teapot1.transform.scale       = glm::vec3{1.0f, 1.0f, 1.0f};
    sceneMin = glm::min(sceneMin, min + teapot1.transform.translation);
    sceneMax = glm::max(sceneMax, max + teapot1.transform.translation);
    m_vMyGameObjects.push_back(std::move(teapot1));

    // Object 2: teapot offset to the right
    auto model2 = MyModel::createModelFromFile(m_myDevice, "models/teapot.obj", min, max);
    auto teapot2 = MyGameObject::createGameObject();
    teapot2.model = std::move(model2);
    teapot2.transform.translation = glm::vec3{10.0f, 0.0f, 0.0f};
    teapot2.transform.scale       = glm::vec3{1.0f, 1.0f, 1.0f};
    sceneMin = glm::min(sceneMin, min + teapot2.transform.translation);
    sceneMax = glm::max(sceneMax, max + teapot2.transform.translation);
    m_vMyGameObjects.push_back(std::move(teapot2));

    // Pass union bbox to camera
    m_myCamera.setSceneMinMax(sceneMin, sceneMax);
    m_myCamera.setMode(MyCamera::MYCAMERA_FITALL);
}

void MyApplication::mouseButtonEvent(bool bMouseDown, float posx, float posy)
{
    m_bMouseButtonPress = bMouseDown;
    m_myCamera.setButton(m_bMouseButtonPress, posx, posy);
}

void MyApplication::mouseMotionEvent(float posx, float posy)
{
    m_myCamera.setMotion(m_bMouseButtonPress, posx, posy);
}

void MyApplication::setCameraNavigationMode(MyCamera::MyCameraMode mode)
{
    m_myCamera.setMode(mode);   
}
