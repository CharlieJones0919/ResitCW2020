#pragma once
#include "IMAT3905.h"

//! An enumerated list of possible movement/rotation directions on keyboard input.
const enum MovementDir { Forward, Back, Left, Right }; 

//! A component to allow for the movement and rotation of the entity's TransformComponent which are called in editor.cpp on keypress events.
class KeyboardComponent
{
public:
	//! Default constructor.
	KeyboardComponent() {};
	//! A constructor to set the TransformComponent that should be updated when the movement/rotation functions are ran on keyboard input.
	KeyboardComponent(TransformComponent* transformComp, float speed) { m_transformComp = transformComp; m_speed = speed; };

	static void moveForward() { move(MovementDir::Forward); }; //!< Moves the entity in the direction of it's local forward vector.
	static void moveBack() { move(MovementDir::Back); };	   //!< Moves the entity in the direction of it's local backward vector.
	static void moveLeft() { move(MovementDir::Left); };	   //!< Moves the entity in the direction of it's local left vector.
	static void moveRight() { move(MovementDir::Right); };	   //!< Moves the entity in the direction of it's local right vector.
															
	static void rotateLeft() { rotate(MovementDir::Left); };   //!< Rotates the entity in the direction of it's local sideward vector to the left using its local upward vector.
	static void rotateRight() { rotate(MovementDir::Right); }; //!< Rotates the entity in the direction of it's local sideward vector to the right using its local upward vector.
private:
	// Used to do translations/rotations of the entity relative to worldspace.
	static glm::vec3 wFRONT; //!< The world's forward direction.
	static glm::vec3 wUP;	 //!< The world's upward direction.
	static glm::vec3 wSIDE;  //!< The world's side direction.

	static TransformComponent* m_transformComp; //!< A pointer to the entity's TransformComponent to update to move/rotate it on key presses.
	static float m_speed;	 //!< Speed of movement/rotation. (A scalar for each on frame update).

	//! Translates the entity's TransformComponent in the direction specified by the MovementDir enum parameter.
	static void move(MovementDir direction)
	{
		// Current position and rotation.
		glm::vec3 position = m_transformComp->translation;
		glm::quat rotation = m_transformComp->rotation;

		// Calculate forward and sideward directional vectors relative to the worldspace directions.
		glm::vec3 forward = glm::normalize(rotation * wFRONT);
		glm::vec3 sideward = glm::normalize(rotation * glm::cross(wFRONT, wUP));

		// Calculate new translation depending on desired direction.
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

		// Set new translation to TransformComponent.
		m_transformComp->translation = position;
		m_transformComp->updateTransform();
	};

	//! Rotates the entity's TransformComponent in the direction specified by the MovementDir enum parameter.
	static void rotate(MovementDir direction)
	{
		// Current rotation.
		glm::vec3 rotation = glm::eulerAngles(m_transformComp->rotation);

		// Calculate forward and upward directional vectors relative to the worldspace directions.
		glm::vec3 forward = glm::normalize(m_transformComp->rotation * wFRONT);
		glm::vec3 upward = glm::normalize(m_transformComp->rotation * glm::cross(wSIDE, forward));

		// Calculate new rotation depending on desired direction.
		switch (direction)
		{
		case MovementDir::Left:
			rotation += upward * m_speed;
			break;
		case MovementDir::Right:
			rotation -= upward * m_speed;
			break;
		}

		// Set new rotation to TransformComponent.
		m_transformComp->rotation = glm::quat(rotation);
		m_transformComp->updateTransform();
	};
};