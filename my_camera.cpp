#include "my_camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

// std
#include <cassert>
#include <limits>
#include <iostream>

MyCamera::MyCamera()
{
    // Set a default view looking along -Z toward the origin.
    // _fitAll() will override this once scene bounds are provided.
    setViewTarget(glm::vec3(0.0f, 0.0f, -2.5f), glm::vec3(0.0f, 0.0f, 0.0f));
}

void MyCamera::setOrthographicProjection(
    float left, float right, float top, float bottom, float near, float far)
{
    m_m4ProjectionMatrix = glm::mat4{ 1.0f };
    m_m4ProjectionMatrix[0][0] =  2.f / (right - left);
    m_m4ProjectionMatrix[1][1] =  2.f / (top - bottom);
    m_m4ProjectionMatrix[2][2] = -2.0f / (far - near);
    m_m4ProjectionMatrix[3][0] = -(right + left) / (right - left);
    m_m4ProjectionMatrix[3][1] = -(top + bottom) / (top - bottom);
    m_m4ProjectionMatrix[3][2] = -(far + near) / (far - near);
}

void MyCamera::setPerspectiveProjection(float fovy, float aspect, float near, float far)
{
    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);

    const float tanHalfFovy = tan(fovy / 2.f);
    const float right  =  near * tanHalfFovy * aspect;
    const float left   = -1.0f * right;
    const float bottom =  near * tanHalfFovy;
    const float top    = -1.0f * bottom;

    m_m4ProjectionMatrix = glm::mat4{ 0.0f };
    m_m4ProjectionMatrix[0][0] =  2.0f * near / (right - left);
    m_m4ProjectionMatrix[2][0] =  (right + left) / (right - left);
    m_m4ProjectionMatrix[1][1] =  2.0f * near / (top - bottom);
    m_m4ProjectionMatrix[2][1] =  (top + bottom) / (top - bottom);
    m_m4ProjectionMatrix[2][2] = -1.0f * (far + near) / (far - near);
    m_m4ProjectionMatrix[3][2] = -2.0f * far * near / (far - near);
    m_m4ProjectionMatrix[2][3] = -1.0f;
}

void MyCamera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
{
    const glm::vec3 w{ glm::normalize(direction) };
    const glm::vec3 u{ glm::normalize(glm::cross(up, w)) };
    const glm::vec3 v{ glm::cross(w, u) };

    m_m4ViewMatrix = glm::mat4{ 1.f };
    m_m4ViewMatrix[0][0] = u.x;
    m_m4ViewMatrix[0][1] = u.y;
    m_m4ViewMatrix[0][2] = u.z;
    m_m4ViewMatrix[1][0] = v.x;
    m_m4ViewMatrix[1][1] = v.y;
    m_m4ViewMatrix[1][2] = v.z;
    m_m4ViewMatrix[2][0] = w.x;
    m_m4ViewMatrix[2][1] = w.y;
    m_m4ViewMatrix[2][2] = w.z;
    m_m4ViewMatrix[3][0] = position.x;
    m_m4ViewMatrix[3][1] = position.y;
    m_m4ViewMatrix[3][2] = position.z;

    m_m4ViewMatrix = glm::inverse(m_m4ViewMatrix);
}

void MyCamera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up)
{
    setViewDirection(position, position - target, up);
}

void MyCamera::setViewYXZ(glm::vec3 position, glm::vec3 rotation)
{
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
    const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
    const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };

    m_m4ViewMatrix = glm::mat4{ 1.f };
    m_m4ViewMatrix[0][0] = u.x;
    m_m4ViewMatrix[1][0] = u.y;
    m_m4ViewMatrix[2][0] = u.z;
    m_m4ViewMatrix[0][1] = v.x;
    m_m4ViewMatrix[1][1] = v.y;
    m_m4ViewMatrix[2][1] = v.z;
    m_m4ViewMatrix[0][2] = w.x;
    m_m4ViewMatrix[1][2] = w.y;
    m_m4ViewMatrix[2][2] = w.z;
    m_m4ViewMatrix[3][0] = -glm::dot(u, position);
    m_m4ViewMatrix[3][1] = -glm::dot(v, position);
    m_m4ViewMatrix[3][2] = -glm::dot(w, position);
}

void MyCamera::setMode(MyCameraMode mode)
{
    m_eMode = mode;
    std::cout << "MyCamera::setMode = " << (int)mode << std::endl;

    if (m_eMode == MYCAMERA_FITALL)
    {
        std::cout << "Fit All" << std::endl;
        _fitAll();
    }
}

void MyCamera::setSceneMinMax(glm::vec3 min, glm::vec3 max)
{
    m_vSceneMin = min;
    m_vSceneMax = max;
}

void MyCamera::setButton(bool buttonPress, float x, float y)
{
    m_m4TempTransform = glm::mat4{ 1.0f };

    if (buttonPress)
    {
        std::cout << "Mouse button pressed" << std::endl;
        m_vCurrPos.x = x;
        m_vCurrPos.y = y;
        m_vPrevPos = m_vCurrPos;
        m_bMoving = true;
    }
    else
    {
        std::cout << "Mouse button released" << std::endl;
        m_bMoving = false;
    }
}

void MyCamera::setMotion(bool buttonPress, float x, float y)
{
    if (!m_bMoving)
        return;

    m_vCurrPos.x = x;
    m_vCurrPos.y = y;

    glm::vec2 delta = m_vCurrPos - m_vPrevPos;
    float mag = glm::length(delta);
    if (mag < 1.0e-6f) return;

    if (m_eMode == MYCAMERA_ROTATE)
    {
        std::cout << "  Rotating..." << std::endl;
        _rotate(delta.x, delta.y);
    }
    else if (m_eMode == MYCAMERA_PAN)
    {
        std::cout << "  Panning..." << std::endl;
        _pan(delta.x, delta.y);
    }
    else if (m_eMode == MYCAMERA_ZOOM)
    {
        std::cout << "  Zooming..." << std::endl;
        _zoom(delta.x, delta.y);
    }
    else if (m_eMode == MYCAMERA_TWIST)
    {
        std::cout << "  Twisting..." << std::endl;
        _twist(delta.x, delta.y);
    }

    // --------------------------------------------------------------------------
    // Combine m_m4TempTransform with m_m4ViewMatrix: new_view = m5*m4*m3*m2*m1
    //
    //  m1 = old view matrix           (Step 3: previous transform)
    //  m2 = centerInverse             (Step 2: shift scene center to origin)
    //  m3 = rotation-only part        (Step 1: decompose TempTransform)
    //  m4 = center                    (Step 2: shift scene center back)
    //  m5 = translation-only part     (Step 1: decompose TempTransform)
    //
    // For pure-rotation TempTransform (Rotate/Twist):  m5=I → rotates around center
    // For pure-translation TempTransform (Pan/Zoom):   m3=I → center*I*centerInv = I
    // --------------------------------------------------------------------------

    // Step 1: decompose TempTransform into rotation-only and translation-only
    glm::mat4 rotateOnly = m_m4TempTransform;
    rotateOnly[3][0] = 0.0f;
    rotateOnly[3][1] = 0.0f;
    rotateOnly[3][2] = 0.0f;

    glm::mat4 translateOnly = glm::mat4{ 1.0f };
    translateOnly[3][0] = m_m4TempTransform[3][0];
    translateOnly[3][1] = m_m4TempTransform[3][1];
    translateOnly[3][2] = m_m4TempTransform[3][2];

    // Step 2: center and centerInverse translation matrices
    glm::vec3 center = (m_vSceneMin + m_vSceneMax) * 0.5f;

    glm::mat4 centerMat = glm::mat4{ 1.0f };
    centerMat[3][0] =  center.x;
    centerMat[3][1] =  center.y;
    centerMat[3][2] =  center.z;

    glm::mat4 centerInverseMat = glm::mat4{ 1.0f };
    centerInverseMat[3][0] = -center.x;
    centerInverseMat[3][1] = -center.y;
    centerInverseMat[3][2] = -center.z;

    // Step 3 + combine: m5 * m4 * m3 * m2 * m1
    m_m4ViewMatrix = translateOnly * centerMat * rotateOnly * centerInverseMat * m_m4ViewMatrix;

    m_vPrevPos = m_vCurrPos;
}

// ---------------------------------------------------------------------------
// _pan — translate camera in screen X / Y
// ---------------------------------------------------------------------------
void MyCamera::_pan(float dx, float dy)
{
    m_m4TempTransform = glm::mat4{ 1.0f };

    // Scale pan speed proportional to current camera distance (depth)
    float d = glm::abs(m_m4ViewMatrix[3][2]);
    if (d < 0.1f) d = 0.1f;
    float scale = d * 0.7f;

    m_m4TempTransform[3][0] = -dx * scale;
    m_m4TempTransform[3][1] = -dy * scale;
}

// ---------------------------------------------------------------------------
// _zoom — dolly the camera along the view Z axis
// ---------------------------------------------------------------------------
void MyCamera::_zoom(float dx, float dy)
{
    m_m4TempTransform = glm::mat4{ 1.0f };

    float d = m_m4ViewMatrix[3][2];  // negative = in front of camera
    float m = 1.0f + dy * 0.7f;    
    float delta_d = d * m - d;

    m_m4TempTransform[3][2] = delta_d;
}

// ---------------------------------------------------------------------------
// _rotate — trackball rotation around the scene center
// ---------------------------------------------------------------------------
void MyCamera::_rotate(float dx, float dy)
{
    glm::vec3 sx, sy, sz, axis, tb_axis;
    float mag = sqrtf(dx * dx + dy * dy);
    float tb_angle = mag * 200.0f;  // scaling factor (degrees per pixel-distance)

    axis.x = dy;   // vertical drag   → rotate about screen X
    axis.y = dx;   // horizontal drag → rotate about screen Y
    axis.z = 0.0f;

    _getScreenXYZ(sx, sy, sz);
    tb_axis = sx * axis.x + sy * axis.y + sz * axis.z;

    _atRotate(tb_axis.x, tb_axis.y, tb_axis.z, tb_angle);
}

// ---------------------------------------------------------------------------
// _atRotate — Rodrigues rotation matrix around an arbitrary axis
// ---------------------------------------------------------------------------
void MyCamera::_atRotate(float x, float y, float z, float angle)
{
    float rad    = angle * glm::pi<float>() / 180.0f;
    float cosAng = cosf(rad);
    float sinAng = sinf(rad);

    glm::vec3 axis(x, y, z);
    axis = glm::normalize(axis);

    m_m4TempTransform = glm::mat4{ 1.0f };

    float x2 = axis.x * axis.x;
    float xy = axis.x * axis.y;
    float xz = axis.x * axis.z;
    float y2 = axis.y * axis.y;
    float yz = axis.y * axis.z;
    float z2 = axis.z * axis.z;

    m_m4TempTransform[0][0] = x2 + (cosAng * (1 - x2));
    m_m4TempTransform[0][1] = xy - (cosAng * xy) + (sinAng * axis[2]);
    m_m4TempTransform[0][2] = xz - (cosAng * xz) - (sinAng * axis[1]);

    m_m4TempTransform[1][0] = xy - (cosAng * xy) - (sinAng * axis[2]);
    m_m4TempTransform[1][1] = y2 + (cosAng * (1 - y2));
    m_m4TempTransform[1][2] = yz - (cosAng * yz) + (sinAng * axis[0]);

    m_m4TempTransform[2][0] = xz - (cosAng * xz) + (sinAng * axis[1]);
    m_m4TempTransform[2][1] = yz - (cosAng * yz) - (sinAng * axis[0]);
    m_m4TempTransform[2][2] = z2 + (cosAng * (1 - z2));

    m_m4TempTransform[3][3] = 1.0f;
}

// ---------------------------------------------------------------------------
// _twist — rotate the camera around the screen Z axis
// ---------------------------------------------------------------------------
void MyCamera::_twist(float dx, float dy)
{
    m_m4TempTransform = glm::mat4{ 1.0f };

    float delta_theta = dx * 7.0f;

    glm::vec3 sx, sy, sz;
    _getScreenXYZ(sx, sy, sz);

    _atRotate(sz.x, sz.y, sz.z, delta_theta);
}

// ---------------------------------------------------------------------------
// _fitAll — frame the entire scene in the view
// ---------------------------------------------------------------------------
void MyCamera::_fitAll()
{
    m_m4TempTransform = glm::mat4{ 1.0f };

    // Compute scene center and bounding-sphere radius
    glm::vec3 center = (m_vSceneMin + m_vSceneMax) * 0.5f;
    glm::vec3 size   = m_vSceneMax - m_vSceneMin;
    float radius     = glm::length(size) * 0.5f;

    // Required viewing distance for 50-degree FOV (half-angle = 25 degrees)
    // Add a small margin so nothing clips at the edges
    float dist = (radius / tanf(glm::radians(25.0f))) + radius * 0.1f;

    // Step 1: place camera dist units along the camera -Z axis
    m_m4TempTransform[3][2] = -dist;

    // Step 2: maintain the current view rotation (zero out the translation column)
    glm::mat4 rotateOnlyMatrix = m_m4ViewMatrix;
    rotateOnlyMatrix[3][0] = 0.0f;
    rotateOnlyMatrix[3][1] = 0.0f;
    rotateOnlyMatrix[3][2] = 0.0f;

    // Step 3: inverse-center — shift scene center to the view origin
    glm::mat4 centerInverseMat = glm::mat4{ 1.0f };
    centerInverseMat[3][0] = -center.x;
    centerInverseMat[3][1] = -center.y;
    centerInverseMat[3][2] = -center.z;

    // Combine: m4 * m3 * m2 * m1
    //   m1 = centerInverse       (center scene at view origin)
    //   m2 = rotateOnly          (preserve existing orientation)
    //   m3 = m_m4TempTransform   (camera distance along -Z)
    m_m4ViewMatrix = m_m4TempTransform * rotateOnlyMatrix * centerInverseMat;
}

// ---------------------------------------------------------------------------
// _getScreenXYZ — extract world-space screen axes from the view matrix
// ---------------------------------------------------------------------------
void MyCamera::_getScreenXYZ(glm::vec3& sx, glm::vec3& sy, glm::vec3& sz)
{
    sx.x = m_m4ViewMatrix[0][0];
    sx.y = m_m4ViewMatrix[1][0];
    sx.z = m_m4ViewMatrix[2][0];
    sy.x = m_m4ViewMatrix[0][1];
    sy.y = m_m4ViewMatrix[1][1];
    sy.z = m_m4ViewMatrix[2][1];
    sz.x = m_m4ViewMatrix[0][2];
    sz.y = m_m4ViewMatrix[1][2];
    sz.z = m_m4ViewMatrix[2][2];
}