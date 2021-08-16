#pragma once
#include "IMAT3905.h"

const enum AIBehaviour { Loop, Wander, Pause };

class AIComponent
{
public:
	int behaviourNum = 0;

	//! Default constructor.
	AIComponent() { };

	AIComponent(TransformComponent* transformComp, AIBehaviour startBehaviour, float speed, glm::ivec2 wpMinBounds, glm::ivec2 wpMaxBounds, int numDesiredWpts) : m_transformComp(transformComp), m_currentBehaviour(startBehaviour), m_speed(speed)
	{
		behaviourNum = static_cast<int>(m_currentBehaviour);
		generateRandomWaypoints(wpMinBounds, wpMaxBounds, numDesiredWpts);
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

	void update(float timestep)
	{		
		switch (m_currentBehaviour)
		{
		case AIBehaviour::Pause: 
			break;
		case AIBehaviour::Loop: loopBehaviour(timestep);
			break;
		case AIBehaviour::Wander: //wanderBehaviour(timestep);
			break;
		}
	};

	void updateAIComponent()
	{
		m_currentBehaviour = AIBehaviour(behaviourNum);
	};
private:
	TransformComponent* m_transformComp;
	float m_speed;
	AIBehaviour m_currentBehaviour;

	std::vector<std::pair<glm::ivec2, bool>> m_waypoints;
	int m_currentWPTarget = 0;
	float m_foundDist = 0.05f;

	void pauseBehaviour(float timestep)
	{
	};

	void loopBehaviour(float timestep)
	{
		if (!m_waypoints[m_currentWPTarget].second)
		{
			if (!rotatingTowardsTarget(timestep))
			{
				if (!movingTowardsTarget(timestep))
				{
					return;
				}
			}
		}
		else
		{
			if (m_currentWPTarget < m_waypoints.size())
			{
				m_waypoints[m_currentWPTarget].second = true;
				m_currentWPTarget++;
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
	};

	void wanderBehaviour(float timestep)
	{

	};

	bool rotatingTowardsTarget(float timestep)
	{
		glm::vec3 position = (*m_transformComp).translation;
		glm::vec3 rotation = glm::eulerAngles((*m_transformComp).rotation);
		glm::vec3 waypointPos(m_waypoints[m_currentWPTarget].first.x, position.y, m_waypoints[m_currentWPTarget].first.y);

		glm::vec3 forward = glm::normalize(rotation * glm::vec3(0.0f, 0.0f, -1.0f));
		glm::vec3 upward = glm::normalize(rotation * glm::cross(forward, glm::vec3(0.0f, 0.0f, -1.0f)));

		glm::vec3 offsetToTarget = waypointPos - position;
		float angleToTarget = acos(glm::dot(forward, glm::normalize(offsetToTarget)));

		if ((angleToTarget) > 0.5f || (angleToTarget < -0.5f))
		{
			rotation -= upward * (angleToTarget * m_speed);

			(*m_transformComp).rotation = glm::quat(rotation);
			(*m_transformComp).updateTransform();
			return false;
		}
		else 
		{ 
			return true;
		}
	};

	bool movingTowardsTarget(float timestep)
	{
		glm::vec3 position = (*m_transformComp).translation;
		glm::vec3 rotation = glm::eulerAngles((*m_transformComp).rotation);
		glm::ivec2 waypointPos = m_waypoints[m_currentWPTarget].first;

		glm::vec3 forward = glm::normalize(rotation * glm::vec3(0.0f, 0.0f, -1.0f));

		float distanceToWpnt = sqrt(pow(position.x - waypointPos.x, 2) + pow(position.z - waypointPos.y, 2));

		if ((distanceToWpnt < m_foundDist) && (distanceToWpnt > -m_foundDist))
		{
			return true;
		}
		else
		{
			position += forward * m_speed;
			(*m_transformComp).translation = position;
			(*m_transformComp).updateTransform();
			return false;
		}

	};

	void generateRandomWaypoints(glm::ivec2 wpMinBounds, glm::ivec2 wpMaxBounds, int numWaypoints)
	{
		m_waypoints.reserve(numWaypoints);

		for (int i = 0; i < numWaypoints; i++)
		{
			int randX = rand() % (wpMaxBounds.x - wpMinBounds.x);
			int randY = rand() % (wpMaxBounds.y - wpMinBounds.y);

			m_waypoints.push_back(std::pair<glm::ivec2, bool>(glm::ivec2(randX, randY), false));
		}
	};
};