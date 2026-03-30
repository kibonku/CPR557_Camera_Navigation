#ifndef __MY_GAMEOBJECT_H__
#define __MY_GAMEOBJECT_H__

#include "my_model.h"
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>

struct TransformComponent
{
	glm::vec3 translation{ 0.0f, 0.0f, 0.0f };
	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
	glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };

	// Matrix corresponds to the overall transformation - scale * Rz * Rx * Ry * transform (row, pitch, yaw)
	// Rotation conventation uses Tait-Bryan angles with axis order Y(1), X(2), Z(3)
	glm::mat4 mat4() 
	{	
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);

		// This is the order of the mulipilcation
		// Rz * Rx * Ry
		return glm::mat4{
			{
				scale.x * (c1 * c3 + s1 * s2 * s3),
				scale.x * (c2 * s3),
				scale.x * (c1 * s2 * s3 - c3 * s1),
				0.0f,
			},
			{
				scale.y * (c3 * s1 * s2 - c1 * s3),
				scale.y * (c2 * c3),
				scale.y * (c1 * c3 * s2 + s1 * s3),
				0.0f,
			},
			{
				scale.z * (c2 * s1),
				scale.z * (-s2),
				scale.z * (c1 * c2),
				0.0f,
			},
			{translation.x, translation.y, translation.z, 1.0f} };

		// This is the order of the mulipilcation
		// Ry * Rx * Rz
		/*return glm::mat4{
			{
				scale.x * (c1 * c3 - s1 * s2 * s3),
				scale.x * (c1 * s3 - c3 * s1 * s2),  
				scale.x * (c2 * s1) * -1.0f,
				0.0f,
			},
			{
				scale.y * (c2 * s3) * -1.0f,
				scale.y * (c2 * c3),
				scale.y * (s2), 
				0.0f,
			},
			{
				scale.z * (c1 * s2 * s3 + c3 * s1),
				scale.z * (s1 * s3 - c1 * c3 * s2),
				scale.z * (c1 * c2),
				0.0f,
			},
			{translation.x, translation.y, translation.z, 1.0f} };*/
	}
};

class MyGameObject
{
public:
	using id_t = unsigned int;

	static MyGameObject createGameObject()
	{
		static id_t currentID = 0;
		return MyGameObject{ currentID++ };
	}

	MyGameObject(const MyGameObject&) = delete;
	MyGameObject& operator=(const MyGameObject&) = delete;
	MyGameObject(MyGameObject&&) = default;
	MyGameObject& operator=(MyGameObject&&) = default;

	id_t                     getID() const { return m_iID; }
	std::shared_ptr<MyModel> model{};
	glm::vec3                color{};
	TransformComponent       transform{};

private:
	MyGameObject(id_t objID) : m_iID{ objID } {}
	id_t m_iID;
};

#endif

