#pragma once
#include "IMAT3905.h"

const enum MovementDir { Forward, Back, Left, Right };

class KeyboardComponent
{
public:
	KeyboardComponent() {};
	KeyboardComponent(TransformComponent& startTransform, float speed) : m_speed(speed) 
	{ 
		updateDirections(startTransform.rotation);
	};

	void move(TransformComponent& transform, MovementDir direction)
	{
		updateDirections(transform.rotation);

		switch (direction)
		{
		case MovementDir::Forward:
			transform.translation += m_forward * m_speed;
			break;
		case MovementDir::Back:
			transform.translation -= m_forward * m_speed;
			break;
		case MovementDir::Left:
			transform.translation -= m_sideward * m_speed;
			break;
		case MovementDir::Right:
			transform.translation += m_sideward * m_speed;
			break;
		}

		transform.updateTransform();
	};

	//static void move(MovementDir direction)
	//{
	//	m_currentPos = m_transform.translation;
	//	m_currentRot = m_transform.rotation;
	//	updateDirections();

	//	switch (direction)
	//	{
	//	case MovementDir::Forward:
	//		m_currentPos = m_forward * m_speed;
	//		break;
	//	case MovementDir::Back:
	//		m_currentPos = -m_forward * m_speed;
	//		break;
	//	case MovementDir::Left:
	//		m_currentPos = -m_sideward * m_speed;
	//		break;
	//	case MovementDir::Right:
	//		m_currentPos = m_sideward * m_speed;
	//		break;
	//	}
	//	updateDirections();

	//	m_transform.translation = m_currentPos;
	//	m_transform.rotation = m_currentRot;
	//	m_transform.updateTransform();
	//};

	//void rotate(MovementDir direction)
	//{
	//	m_currentRot = m_transform.rotation;

	//	switch (direction)
	//	{
	//	case MovementDir::Forward:
	//		m_currentRot.x *= m_speed;
	//		break;
	//	case MovementDir::Back:
	//		m_currentRot.x *= -m_speed;
	//		break;
	//	case MovementDir::Left:
	//		m_currentRot.y *= -m_speed;
	//		break;
	//	case MovementDir::Right:
	//		m_currentRot.y *= m_speed;
	//		break;
	//	}
	//	updateDirections();
	//	m_transform.rotation = m_currentRot;
	//};

private:
	float m_speed;
	glm::vec3 m_forward = glm::vec3(0.0f, 0.0f, -1.0f );
	glm::vec3 m_sideward;
	glm::vec3 m_upward = glm::vec3(0.0f, 1.0f, 0.0f);

	void updateDirections(glm::quat rotation)
	{
		// Calculate the new m_forward vector based on the entity's current rotation.
		if (rotation.y != 0.0f)
		{ 
			glm::vec3 newForward;
			newForward.x = cos(rotation.z) * cos(rotation.x);
			newForward.y = sin(rotation.x);
			newForward.z = sin(rotation.z) * cos(rotation.x);

			m_forward = glm::normalize(newForward);
		}

		// Set the directional vectors based on the newly calculated forward from current rotation.
		m_sideward = glm::normalize(glm::cross(m_forward, m_upward));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower MovementDir.	
		m_upward = glm::normalize(glm::cross(m_sideward, m_forward));
	};
};