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
    // 1. Load the teapot model and get its bounding box
    glm::vec3 sceneMin, sceneMax;
    auto model = MyModel::createModelFromFile(m_myDevice, "models/teapot.obj", sceneMin, sceneMax);

    // 2. Create a game object and attach the model
    auto teapot = MyGameObject::createGameObject();
    teapot.model = std::move(model);
    teapot.transform.translation = glm::vec3{ 0.0f, 0.0f, 0.0f };
    teapot.transform.scale       = glm::vec3{ 1.0f, 1.0f, 1.0f };
    m_vMyGameObjects.push_back(std::move(teapot));

    // 3. Tell the camera about the scene bounds
    m_myCamera.setSceneMinMax(sceneMin, sceneMax);

    // 4. Call Fit All to set the initial camera position
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
