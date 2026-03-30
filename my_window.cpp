#include "my_window.h"
#include "my_application.h"
#include "my_camera.h"
#include <stdexcept>
#include <iostream>

MyWindow::MyWindow(int w, int h, std::string name) : 
	m_iWidth(w),
	m_iHeight(h),
	m_sWindowName(name),
	m_pWindow(nullptr),
	m_bframeBufferResize(false),
	m_pMyApplication(nullptr)
{
	_initWindow();
}

MyWindow::~MyWindow()
{
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
}

void MyWindow::_initWindow()
{
	glfwInit();

	// GLFW was designed initially to create OpenGL context by default.
	// By setting GLFW_CLIENT_API to GLFW_NO_API, it tells GLFW NOT to create OpenGL context 
	// (because we are going to use Vulkan)
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// Need to handle the window resizing in a specical way later in Vulkan code
	// so we need to set it to false initally until frameBufferResizeCallback is set
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	m_pWindow = glfwCreateWindow(m_iWidth, m_iHeight, m_sWindowName.c_str(), nullptr, nullptr);

	// For the call back function to use this pointer
	glfwSetWindowUserPointer(m_pWindow, this);

	// Register viewport resize callback
	glfwSetFramebufferSizeCallback(m_pWindow, s_frameBufferResizeCallback);

	// Register keyboard callback	
	glfwSetKeyCallback(m_pWindow, s_keyboardCallback);

	// Register mouse button callback
	glfwSetMouseButtonCallback(m_pWindow, s_mouseButtonCallback);
}

void MyWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
	if (glfwCreateWindowSurface(instance, m_pWindow, nullptr, surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window");
	}
}

void MyWindow::s_frameBufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto mywindow = reinterpret_cast<MyWindow*>(glfwGetWindowUserPointer(window));
	mywindow->m_bframeBufferResize = true;
	mywindow->m_iWidth = width;
	mywindow->m_iHeight = height;
}

void MyWindow::s_keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Handle keyboard events
	auto mywindow = reinterpret_cast<MyWindow*>(glfwGetWindowUserPointer(window));
	
	if ((key == GLFW_KEY_C || key == GLFW_KEY_O || key == GLFW_KEY_ESCAPE ||
		 key == GLFW_KEY_F || key == GLFW_KEY_R || key == GLFW_KEY_P || key == GLFW_KEY_Z || key == GLFW_KEY_T))
	{
		if (action == GLFW_PRESS)
			mywindow->keyboardEvent(key, true);
		if (action == GLFW_RELEASE)
			mywindow->keyboardEvent(key, false);
	}
}

void MyWindow::pollEvents()
{
	glfwPollEvents();

	double xpos = 0.0, ypos = 0.0;
	glfwGetCursorPos(m_pWindow, &xpos, &ypos);

	float fMousePos[2];
	fMousePos[0] = (float)xpos / (float)m_iWidth;
	fMousePos[1] = (float)ypos / (float)m_iHeight;

	m_pMyApplication->mouseMotionEvent(fMousePos[0], fMousePos[1]);
}

void MyWindow::waitEvents()
{
	glfwWaitEvents();
}

const char** MyWindow::getRequiredInstanceExtensions(uint32_t* extensionCount)
{
	return glfwGetRequiredInstanceExtensions(extensionCount);
}

void MyWindow::bindMyApplication(MyApplication* pMyApplication)
{
	m_pMyApplication = pMyApplication;
}

void MyWindow::keyboardEvent(int key, bool bKeyDown)
{
	if (m_pMyApplication == nullptr) return;

	if (key == GLFW_KEY_C && bKeyDown)
	{
		m_pMyApplication->switchProjectionMatrix();
	}
	else if (key == GLFW_KEY_O && bKeyDown)
	{
		m_pMyApplication->switchProjectionMatrix();
	}
	else if (key == GLFW_KEY_ESCAPE && bKeyDown)
	{
		glfwSetWindowShouldClose(m_pWindow, 1);
	}
	else if (key == GLFW_KEY_F && bKeyDown)
	{
		m_pMyApplication->setCameraNavigationMode(MyCamera::MYCAMERA_FITALL);
	}
	else if (key == GLFW_KEY_R && bKeyDown)
	{
		m_pMyApplication->setCameraNavigationMode(bKeyDown ? MyCamera::MYCAMERA_ROTATE : MyCamera::MYCAMERA_NONE);
	}
	else if (key == GLFW_KEY_P && bKeyDown)
	{
		m_pMyApplication->setCameraNavigationMode(bKeyDown ? MyCamera::MYCAMERA_PAN : MyCamera::MYCAMERA_NONE);
	}
	else if (key == GLFW_KEY_Z && bKeyDown)
	{
		m_pMyApplication->setCameraNavigationMode(bKeyDown ? MyCamera::MYCAMERA_ZOOM : MyCamera::MYCAMERA_NONE);
	}
	else if (key == GLFW_KEY_T && bKeyDown)
	{
		m_pMyApplication->setCameraNavigationMode(bKeyDown ? MyCamera::MYCAMERA_TWIST : MyCamera::MYCAMERA_NONE);
	}
}

void MyWindow::s_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	auto mywindow = reinterpret_cast<MyWindow*>(glfwGetWindowUserPointer(window));

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	float fMousePos[2];
	fMousePos[0] = (float)xpos / (float)mywindow->m_iWidth;
	fMousePos[1] = (float)ypos / (float)mywindow->m_iHeight;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		mywindow->m_pMyApplication->mouseButtonEvent(true, fMousePos[0], fMousePos[1]);
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		mywindow->m_pMyApplication->mouseButtonEvent(false, fMousePos[0], fMousePos[1]);
	}
}