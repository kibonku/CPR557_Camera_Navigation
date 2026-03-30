#ifndef __MY_APPLICATION_H__
#define __MY_APPLICATION_H__

#include "my_window.h"
#include "my_device.h"
#include "my_renderer.h"
#include "my_game_object.h"
#include "my_camera.h"

#include <memory>
#include <vector>

class MyApplication 
{
public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

	MyApplication();

	void run();

	// Assignment
	void switchProjectionMatrix();
	void mouseButtonEvent(bool bMouseDown, float posx, float posy);
	void mouseMotionEvent(float posx, float posy);
	void setCameraNavigationMode(MyCamera::MyCameraMode mode);

private:
	void _loadGameObjects();

	MyWindow                  m_myWindow{ WIDTH, HEIGHT, "Assignment 4" };
	MyDevice                  m_myDevice{ m_myWindow };
	MyRenderer                m_myRenderer{ m_myWindow, m_myDevice };

	std::vector<MyGameObject> m_vMyGameObjects;
	MyCamera                  m_myCamera{};
	bool                      m_bPerspectiveProjection;
	bool                      m_bMouseButtonPress = false;
};

#endif

