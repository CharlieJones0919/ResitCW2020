#pragma once
#include "IMAT3905.h"
#include <iostream>

const enum AIBehaviour { Loop, Wander, Pause };

class AIComponent
{
public:
	int behaviourNum = 0;

	//! Default constructor.
	AIComponent() { };

	AIComponent(AIBehaviour startBehaviour, float mSpeed, float tSpeed, glm::ivec2 wpMinBounds, glm::ivec2 wpMaxBounds, int numDesiredWpts) : m_currentBehaviour(startBehaviour), m_moveSpeed(mSpeed), m_turnSpeed(tSpeed)
	{
		behaviourNum = static_cast<int>(m_currentBehaviour);

		m_waypoints.reserve(numDesiredWpts);
		for (int i = 0; i < numDesiredWpts; i++)
		{
			m_waypoints.push_back(std::pair<glm::ivec2, bool>(glm::ivec2(generateRandomInt(wpMinBounds.x, wpMaxBounds.x), generateRandomInt(wpMinBounds.y, wpMaxBounds.y)), false));
		}

		updateAIComponent();
	};

	//AIComponent(TransformComponent* transformComp, AIBehaviour startBehaviour, float speed, std::vector<glm::ivec2> wayPts)
	//{ 
	//	m_transformComp = transformComp;
	//	m_speed = speed;
	//	m_currentBehaviour = startBehaviour;

	//	if (wayPts.size() != 0) 
	//	{
	//		m_waypoints.reserve(wayPts.size());
	//		for (auto& point : wayPts)
	//		{
	//			m_waypoints.push_back(std::pair<glm::ivec2, bool>(point, false));
	//		}
	//	}
	//	else { m_currentBehaviour = AIBehaviour::Pause; }
	//};

	int getNumWaypoints() { return m_waypoints.size(); };
	std::vector<std::pair<glm::ivec2, bool>> getWaypoints() { return m_waypoints; };
	glm::ivec2 getWaypoint(int id) { return m_waypoints[id].first; };

	void addWaypoint(glm::ivec2 newWP) 
	{ 
		m_waypoints.reserve(m_waypoints.size() + 1);
		m_waypoints.push_back(std::pair<glm::ivec2, bool>(newWP, false));
	};
	void addWaypoint(float wpX, float wpZ)
	{
		m_waypoints.reserve(m_waypoints.size() + 1);
		m_waypoints.push_back(std::pair<glm::ivec2, bool>(glm::ivec2(wpX, wpZ), false));
	};
	void addWaypoint(std::vector<glm::ivec2> wayPts)
	{
		m_waypoints.reserve(m_waypoints.size() + wayPts.size());
		for (auto& point : wayPts)
		{
			m_waypoints.push_back(std::pair<glm::ivec2, bool>(point, false));
		}
	};


	void editWaypoint(int pointNum, int newPosX, int newPosZ) { editWaypoint(pointNum, glm::ivec2(newPosX, newPosZ)); };
	void editWaypoint(int pointNum, glm::ivec2 newPos)
	{
		if ((pointNum > 0) && (pointNum < m_waypoints.size()))
		{
			m_waypoints[pointNum].first = newPos;
			m_waypoints[pointNum].second = false;
		}
	};

	void deleteWaypoint(int wpNum)
	{
		//Delete waypoint.
	};

	void update(float timestep, TransformComponent* transformComp)
	{		
		switch (m_currentBehaviour)
		{
		case AIBehaviour::Pause: 
			break;
		case AIBehaviour::Loop: loopBehaviour(timestep, transformComp);
			break;
		case AIBehaviour::Wander: wanderBehaviour(timestep, transformComp);
			break;
		}
	};

	void updateAIComponent()
	{
		m_currentBehaviour = AIBehaviour(behaviourNum);
	};
private:
	float m_moveSpeed;
	float m_turnSpeed;
	AIBehaviour m_currentBehaviour;

	std::vector<std::pair<glm::ivec2, bool>> m_waypoints;
	static int m_currentWPTarget;
	float m_foundDist;

	glm::vec3 m_cWaypntPos;
	glm::vec3 m_cForward;
	glm::vec3 m_cUpward;
	float m_angleToTarget;

	int generateRandomInt(int min, int max)
	{
		return min + (rand() % max);
	};

	void calcCurrentVectors(TransformComponent* transformComp)
	{
		m_cWaypntPos = glm::vec3(m_waypoints[m_currentWPTarget].first.x, transformComp->translation.y, m_waypoints[m_currentWPTarget].first.y);
		m_cForward = glm::normalize(transformComp->rotation * glm::vec3(0.0f, 0.0f, -1.0f));
		m_cUpward = glm::normalize(transformComp->rotation * glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), m_cForward));
	};

	bool facingTarget(TransformComponent* transformComp)
	{
		glm::vec3 m_offsetToTarget = m_cWaypntPos - transformComp->translation;
		m_angleToTarget = acos(glm::dot(m_cForward, glm::normalize(m_offsetToTarget)));

		if ((m_angleToTarget < 0.05f) && (m_angleToTarget > -0.05f))
		{
			return true;
		}
		else { return false; }
	};

	bool reachedTarget(TransformComponent* transformComp)
	{
		float distanceToWpnt = sqrt(pow(transformComp->translation.x - m_cWaypntPos.x, 2) + pow(transformComp->translation.z - m_cWaypntPos.z, 2));

		if (distanceToWpnt < 0.05f)
		{
			return true;
		}
		else { return false; }
	};

	void turnToTarget(float timestep, TransformComponent* transformComp)
	{
		glm::vec3 newRot = glm::eulerAngles(transformComp->rotation);
		
		if (m_angleToTarget < 0.0f)
		{
			newRot += m_cUpward * (m_turnSpeed * timestep);
		}
		else if (m_angleToTarget > 0.0f)
		{
			newRot -= m_cUpward * (m_turnSpeed * timestep);
		}

		transformComp->rotation = glm::quat(newRot);
		transformComp->updateTransform();
	};

	void moveToTarget(float timestep, TransformComponent* transformComp)
	{
		glm::vec3 newPos = transformComp->translation;		
		newPos += m_cForward * (m_moveSpeed * timestep);

		transformComp->translation = newPos;
		transformComp->updateTransform();
	};


	void pauseBehaviour(float timestep)
	{
	};

	void loopBehaviour(float timestep, TransformComponent* transformComp)
	{
		calcCurrentVectors(transformComp);

		std::cout << "Current WP: " << m_currentWPTarget << std::endl;
		std::cout << "WP Pos: " << m_waypoints[m_currentWPTarget].first.x << ", " "WP Pos: " << m_waypoints[m_currentWPTarget].first.y << std::endl;
		std::cout << "State: " << m_waypoints[m_currentWPTarget].second << std::endl << std::endl;
		if (!m_waypoints[m_currentWPTarget].second)
		{
			if (facingTarget(transformComp))
			{
				if (reachedTarget(transformComp))
				{
					if (m_currentWPTarget < (m_waypoints.size() - 1))
					{
						m_waypoints[m_currentWPTarget].second = true;
						m_currentWPTarget += 1;
					}
					else
					{
						m_currentWPTarget = 0;
						for (auto& point : m_waypoints)
						{
							point.second = false;
						}
					}
				}
				else
				{
					moveToTarget(timestep, transformComp); 
				}
			}
			else
			{
				turnToTarget(timestep, transformComp);
			}
		}
	};

	void wanderBehaviour(float timestep, TransformComponent* transformComp)
	{

	};
};