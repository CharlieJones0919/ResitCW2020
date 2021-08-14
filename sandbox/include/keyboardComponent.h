#pragma once
#include "IMAT3905.h"

const enum MovementDir { Forward, Back, Left, Right };

class KeyboardComponent
{
public:
	//! Default constructor.
	KeyboardComponent() {};
	//! A constructor to set the 
	KeyboardComponent(TransformComponent* transformComp, float speed) { m_keyTransComp = transformComp; m_speed = speed; };

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

	static TransformComponent* m_keyTransComp;
	static float m_speed;

	static void move(MovementDir direction)
	{
		glm::vec3 position = (*m_keyTransComp).translation;
		glm::quat rotation = (*m_keyTransComp).rotation;

		glm::vec3 m_forward = glm::normalize(rotation * wFRONT);
		glm::vec3 m_sideward = glm::normalize(rotation * glm::cross(wFRONT, wUP));

		switch (direction)
		{
		case MovementDir::Forward:
			position += m_forward * m_speed;
			break;
		case MovementDir::Back:
			position -= m_forward * m_speed;
			break;
		case MovementDir::Left:
			position -= m_sideward * m_speed;
			break;
		case MovementDir::Right:
			position += m_sideward * m_speed;
			break;
		}

		(*m_keyTransComp).translation = position;
		(*m_keyTransComp).updateTransform();
	};

	static void rotate(MovementDir direction)
	{
		glm::vec3 rotation = glm::eulerAngles((*m_keyTransComp).rotation);

		glm::vec3 m_forward = glm::normalize((*m_keyTransComp).rotation * wFRONT);
		glm::vec3 m_upward = glm::normalize((*m_keyTransComp).rotation * glm::cross(wSIDE, m_forward));

		switch (direction)
		{
		case MovementDir::Left:
			rotation += m_upward * m_speed;
			break;
		case MovementDir::Right:
			rotation -= m_upward * m_speed;
			break;
		}

		(*m_keyTransComp).rotation = glm::quat(rotation);
		(*m_keyTransComp).updateTransform();
	};
};