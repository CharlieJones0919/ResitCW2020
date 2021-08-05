#pragma once
#include "IMAT3905.h"

class KeyboardComponent
{
public:
	glm::vec3 m_prevPos;

	//KeyboardComponent() { transform = glm::mat4(1.0f); }
	//
	//
	//void addRotation(glm::vec3& eulerAngles)
	//{
	//	glm::quat rotationDelta(eulerAngles);
	//	rotation *= rotationDelta;
	//}
private:

	glm::quat m_rotationChange;
};