#pragma once
#include "IMAT3905.h"

const enum MovementDir { Forward, Back, Left, Right };

class KeyboardComponent
{
public:
	//! Default constructor.
	KeyboardComponent() {};
	//! A constructor to set the TransformComponent that should be updated when the movement/rotation functions are ran on keyboard input.
	KeyboardComponent(TransformComponent* transformComp, float speed) { m_transformComp = transformComp; m_speed = speed; };

	static void moveForward() { move(MovementDir::Forward); };
	static void moveBack() { move(MovementDir::Back); };
	static void moveLeft() { move(MovementDir::Left); };
	static void moveRight() { move(MovementDir::Right); };

	static void rotateLeft() { rotate(MovementDir::Left); };
	static void rotateRight() { rotate(MovementDir::Right); };
private:
	static glm::vec3 wFRONT;
	static glm::vec3 wUP;
	static glm::vec3 wSIDE;

	static TransformComponent* m_transformComp;
	static float m_speed;

	static void move(MovementDir direction)
	{
		glm::vec3 position = (*m_transformComp).translation;
		glm::quat rotation = (*m_transformComp).rotation;

		glm::vec3 forward = glm::normalize(rotation * wFRONT);
		glm::vec3 sideward = glm::normalize(rotation * glm::cross(wFRONT, wUP));

		switch (direction)
		{
		case MovementDir::Forward:
			position += forward * m_speed;
			break;
		case MovementDir::Back:
			position -= forward * m_speed;
			break;
		case MovementDir::Left:
			position -= sideward * m_speed;
			break;
		case MovementDir::Right:
			position += sideward * m_speed;
			break;
		}

		(*m_transformComp).translation = position;
		(*m_transformComp).updateTransform();
	};

	static void rotate(MovementDir direction)
	{
		glm::vec3 rotation = glm::eulerAngles((*m_transformComp).rotation);

		glm::vec3 forward = glm::normalize((*m_transformComp).rotation * wFRONT);
		glm::vec3 upward = glm::normalize((*m_transformComp).rotation * glm::cross(wSIDE, forward));

		switch (direction)
		{
		case MovementDir::Left:
			rotation += upward * m_speed;
			break;
		case MovementDir::Right:
			rotation -= upward * m_speed;
			break;
		}

		(*m_transformComp).rotation = glm::quat(rotation);
		(*m_transformComp).updateTransform();
	};
};