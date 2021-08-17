#pragma once
#include "IMAT3905.h"
#include <iostream>

const enum AIBehaviour { Loop = 0, Wander = 1, Pause = 2 };

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
	glm::ivec2 getWaypoint(int id) { return m_waypoints[id].first; };
	float getAngle2Target() { return m_angleToTarget; }
	glm::vec3 getDist2Target() { return m_offsetToTarget; }

	bool waypointIsTarget(int id)
	{
		if (id == m_cWPTarget) { return true; }
		else { return false; }
	};

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
		if ((pointNum >= 0) && (pointNum < m_waypoints.size()))
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
		if (m_currentBehaviour != AIBehaviour::Pause)
		{
			findWaypoint(timestep, transformComp, m_currentBehaviour);
		}
	};

	void updateAIComponent()
	{
		if (m_currentBehaviour != behaviourNum)
		{
			m_currentBehaviour = AIBehaviour(behaviourNum);
		}
	};
private:
	float m_foundDist = 0.05f;
	float m_moveSpeed;
	float m_turnSpeed;
	AIBehaviour m_currentBehaviour;
	std::vector<std::pair<glm::ivec2, bool>> m_waypoints;

	static int m_cWPTarget;
	glm::vec3 m_cWaypntPos;
	glm::vec3 m_cForward;
	glm::vec3 m_cUpward;

	static float m_angleToTarget;
	static	glm::vec3 m_offsetToTarget;

	int generateRandomInt(int min, int max)
	{
		return min + (rand() % max);
	};

	float roundTo2Decimal(float amount)
	{
		int val = (amount * 100) + 0.5f;
		return val / 100.0f;
	}
	glm::vec3 roundTo2Decimal(glm::vec3 amount)
	{
		glm::ivec3 val = (amount * glm::vec3(100) + glm::vec3(0.5f));
		return ((glm::vec3)val / glm::vec3(100));
	}

	void calcCurrentVectors(TransformComponent* transformComp)
	{
		m_cWaypntPos = glm::vec3(m_waypoints[m_cWPTarget].first.x, transformComp->translation.y, m_waypoints[m_cWPTarget].first.y);
		m_cForward = glm::normalize(transformComp->rotation * glm::vec3(0.0f, 0.0f, -1.0f));
		m_cUpward = glm::normalize(transformComp->rotation * glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), m_cForward));

		m_offsetToTarget = m_cWaypntPos - transformComp->translation;
		m_angleToTarget = acos(glm::dot(m_cForward, glm::normalize(m_offsetToTarget)));

		m_offsetToTarget = roundTo2Decimal(m_offsetToTarget);
		m_angleToTarget = roundTo2Decimal(m_angleToTarget);
	};

	bool facingTarget(TransformComponent* transformComp)
	{
		if ((m_angleToTarget < m_foundDist) && (m_angleToTarget > -m_foundDist))
		{
			return true;
		}
		else { return false; }
	};

	bool reachedTarget(TransformComponent* transformComp)
	{
		if ((m_offsetToTarget.x < m_foundDist) && (m_offsetToTarget.x > -m_foundDist) &&
			(m_offsetToTarget.z < m_foundDist) && (m_offsetToTarget.z > -m_foundDist))
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

	void findWaypoint(float timestep, TransformComponent* transformComp, AIBehaviour behaviour)
	{
		calcCurrentVectors(transformComp);

		if (!m_waypoints[m_cWPTarget].second)
		{
			if (facingTarget(transformComp))
			{
				if (reachedTarget(transformComp)) { setNextTarget(transformComp, behaviour); }
				else { moveToTarget(timestep, transformComp); }
			}
			else { turnToTarget(timestep, transformComp); }
		}
	}

	void setNextTarget(TransformComponent* transformComp, AIBehaviour behaviour)
	{
		switch (behaviour)
		{
		case AIBehaviour::Loop:
			for (auto& point : m_waypoints)
			{
				point.second = false;
			}

			if (m_cWPTarget < (m_waypoints.size() - 1))
			{
				m_waypoints[m_cWPTarget].second = true;
				m_cWPTarget++;
			}
			else
			{
				m_cWPTarget = 0;
				for (auto& point : m_waypoints)
				{
					point.second = false;
				}

			}
			break;

		case AIBehaviour::Wander:
			
			
			int newWpntTarget = m_cWPTarget;
			while (m_cWPTarget == newWpntTarget)
			{
				newWpntTarget = generateRandomInt(0, m_waypoints.size());
			}
			m_cWPTarget = newWpntTarget;

			if (m_waypoints[m_cWPTarget].second)
			{
				for (auto& point : m_waypoints)
				{
					point.second = false;
				}
			}
			break;
		}
		transformComp->rotation = glm::quat(0, 0, 0, 1);
		transformComp->updateTransform();
	}
};