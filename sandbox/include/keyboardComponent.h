#pragma once
#include "IMAT3905.h"

const enum MovementDir { Forward, Back, Left, Right };

class KeyboardComponent
{
public:
	//! Default constructor.
	KeyboardComponent() {};
	//! A constructor to set the 
	KeyboardComponent(float speed) : m_speed(speed) {};

	void move(TransformComponent& transform, MovementDir direction)
	{
		updateDirections(transform.rotation);
		glm::vec3 newPosition = transform.translation;

		switch (direction)
		{
		case MovementDir::Forward:
			newPosition += m_forward * m_speed;
			break;
		case MovementDir::Back:
			newPosition -= m_forward * m_speed;
			break;
		case MovementDir::Left:
			newPosition -= m_sideward * m_speed;
			break;
		case MovementDir::Right:
			newPosition += m_sideward * m_speed;
			break;
		}
		transform.translation = newPosition;
		transform.updateTransform();
	};

	void rotate(TransformComponent& transform, MovementDir direction)
	{
		updateDirections(transform.rotation);
		glm::vec3 newRotation = glm::eulerAngles(transform.rotation);

		switch (direction)
		{
		case MovementDir::Left:
			newRotation += m_upward * m_speed;

			break;
		case MovementDir::Right:
			newRotation -= m_upward * m_speed;
			break;
		}

		transform.rotation = glm::quat(newRotation);
		transform.updateTransform();
	};

private:
	 glm::vec3 wFRONT = glm::vec3(0.0f, 0.0f, -1.0f);
	 glm::vec3 wUP = glm::vec3(0.0f, 1.0f, 0.0f);
	 glm::vec3 wSIDE = glm::vec3(1.0f, 0.0f, 0.0f);

	float m_speed;
	glm::vec3 m_forward = wFRONT;
	glm::vec3 m_sideward = wSIDE;
	glm::vec3 m_upward = wUP;

	void updateDirections(glm::quat rotation)
	{
		// Calculate the new m_forward vector based on the entity's current rotation.
		m_forward = glm::normalize(rotation * wFRONT);
		m_sideward = glm::normalize(rotation * glm::cross(wFRONT, wUP));
		m_upward = glm::normalize( rotation * glm::cross(wSIDE, m_forward));

		//// Set the directional vectors based on the newly calculated forward from current rotation.
	//	m_forward = glm::normalize(m_forward);
	//	m_sideward = glm::normalize(glm::cross(m_forward, m_upward));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower MovementDir.	
	//	m_upward = glm::normalize(glm::cross(m_sideward, m_forward));
	};
};