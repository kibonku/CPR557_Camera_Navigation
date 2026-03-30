#ifndef __MY_CAMERA_H__
#define __MY_CAMERA_H__

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

class MyCamera 
{
public:

    MyCamera();

    enum MyCameraMode
    {
        MYCAMERA_NONE = 0,
        MYCAMERA_ROTATE,
        MYCAMERA_PAN,
        MYCAMERA_ZOOM,
        MYCAMERA_TWIST,
        MYCAMERA_FITALL
    };

    void setOrthographicProjection(
        float left, float right, float top, float bottom, float near, float far);

    void setPerspectiveProjection(float fovy, float aspect, float near, float far);

    void setViewDirection(glm::vec3 position, glm::vec3 dirction, glm::vec3 up = glm::vec3{ 0.0f, 1.0f, 0.0f });
    void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{ 0.0f, 1.0f, 0.0f });
    void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

    const glm::mat4& projectionMatrix() const { return m_m4ProjectionMatrix; }
    const glm::mat4& viewMatrix()       const { return m_m4ViewMatrix; }

    // Assignment
    void setMode(MyCameraMode mode);
    void setSceneMinMax(glm::vec3 min, glm::vec3 max);
    void setButton(bool buttonPress, float x, float y);
    void setMotion(bool buttonPress, float x, float y);

private:

    void _pan(float dx, float dy);
    void _zoom(float dx, float dy);
    void _rotate(float dx, float dy);
    void _twist(float dx, float dy);
    void _fitAll();

    void _atRotate(float x, float y, float z, float angle);
    void _getScreenXYZ(glm::vec3 &sx, glm::vec3 &sy, glm::vec3 &sz);

    glm::mat4    m_m4ProjectionMatrix{ 1.f };
    glm::mat4    m_m4ViewMatrix{ 1.f };
    glm::mat4    m_m4TempTransform{ 1.0f };

    glm::vec2    m_vCurrPos;
    glm::vec2    m_vPrevPos;

    glm::vec3    m_vSceneMin;
    glm::vec3    m_vSceneMax;

    bool         m_bMoving = false;
    MyCameraMode m_eMode = MYCAMERA_NONE;
};

#endif

