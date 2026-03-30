#ifndef __MY_WINDOW_H__
#define __MY_WINDOW_H__

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// std
#include <string>

class MyApplication;

class MyWindow
{
public:
	MyWindow(int w, int h, std::string name);
	~MyWindow();

	// Cannot do copy or move contrucror or assignment
	// because we have a GLGWwindow pointer
	// C++ rule of five
	MyWindow(const MyWindow&) = delete;
	MyWindow& operator=(const MyWindow&) = delete;
	MyWindow(MyWindow &&) = delete;
	MyWindow& operator=(const MyWindow&&) = delete;

	bool       shouldClose()            { return glfwWindowShouldClose(m_pWindow); }
	VkExtent2D extent()                 { return { static_cast<uint32_t>(m_iWidth), static_cast<uint32_t>(m_iHeight) }; };
	bool       wasWindowResized()       { return m_bframeBufferResize; }
	GLFWwindow* glfwWindow()      const { return m_pWindow; }
	void       resetWindowResizedFlag() { m_bframeBufferResize = false; }

	void       createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
	void       pollEvents();
	void       waitEvents();

	const char** getRequiredInstanceExtensions(uint32_t *extensionCount);

	// Application specific functions
	void       bindMyApplication(MyApplication* pMyApplication);
	void       keyboardEvent(int key, bool bKeyDown);

private:
	static void s_keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void s_frameBufferResizeCallback(GLFWwindow* window, int width, int height);
	static void s_mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	void _initWindow();

	int            m_iWidth;
	int            m_iHeight;
	bool           m_bframeBufferResize = false;

	std::string    m_sWindowName;
	GLFWwindow*    m_pWindow;

	// Application specific member data
	MyApplication* m_pMyApplication;
};

#endif

