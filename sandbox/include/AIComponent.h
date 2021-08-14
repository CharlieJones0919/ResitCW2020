#pragma once
#include "IMAT3905.h"

const enum AIBehaviour { Loop, Wander, Pause };

class AIComponent
{
public:
	//! Default constructor.
	AIComponent() { };

	AIComponent(TransformComponent* transformComp, float speed, glm::vec3 wpLowerBounds, glm::vec3 wpUpperBounds, int numWaypoints)
	{
		m_transformComp = transformComp;
		m_speed = speed;
		generateRandomWaypoints(wpLowerBounds, wpUpperBounds, numWaypoints);
	};

	AIComponent(TransformComponent* transformComp, float speed, std::vector<glm::vec3> wayPts, glm::vec3 wpLowerBounds, glm::vec3 wpUpperBounds)
	{ 
		m_transformComp = transformComp;
		m_speed = speed;

		m_waypoints.reserve(wayPts.size()); 
		for (auto& point : wayPts)
		{
			m_waypoints.push_back(std::pair<glm::vec3, bool>(point, false));
		}

		if (m_waypoints.size() == 0) { m_currentBehaviour = AIBehaviour::Pause; }
	};

	void addWaypoint(glm::vec3 newWP) 
	{ 
		m_waypoints.reserve(m_waypoints.size() + 1);
		m_waypoints.push_back(std::pair<glm::vec3, bool>(newWP, false));
	};
	void addWaypoint(float wp1, float wp2, float wp3)
	{
		m_waypoints.reserve(m_waypoints.size() + 1);
		m_waypoints.push_back(std::pair<glm::vec3, bool>(glm::vec3(wp1, wp2, wp3), false));
	};
	void addWaypoint(std::vector<glm::vec3> wayPts)
	{
		m_waypoints.reserve(m_waypoints.size() + wayPts.size());
		for (auto& point : wayPts)
		{
			m_waypoints.push_back(std::pair<glm::vec3, bool>(point, false));
		}
	};

	void deleteWaypoint(int wpNum)
	{
		//Delete waypoint.
	};

	std::vector<std::pair<glm::vec3, bool>>& getWaypoints() { return m_waypoints; };

	void setBehaviour(AIBehaviour behaviour)
	{
		m_currentBehaviour = behaviour;
	};

	void update(float timestep)
	{
		switch (m_currentBehaviour)
		{
		case AIBehaviour::Pause: return; 
			break;
		case AIBehaviour::Loop: loopBehaviour();
			break;
		case AIBehaviour::Wander: wanderBehaviour();
			break;
		}
	};

private:
	TransformComponent* m_transformComp;
	float m_speed;
	AIBehaviour m_currentBehaviour;

	std::vector<std::pair<glm::vec3, bool>> m_waypoints;
	int m_currentWPTarget = 0;

	void loopBehaviour()
	{
		if (!m_waypoints[m_currentWPTarget].second)
		{
			if (!rotatingTowardsTarget())
			{
				if (!movingTowardsTarget())
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
			}
		}
	}

	void wanderBehaviour()
	{
	
	}


	bool rotatingTowardsTarget()
	{
		glm::vec3 rotation = glm::eulerAngles((*m_transformComp).rotation);

		glm::vec3 forward = glm::normalize(rotation * glm::vec3(0.0f, 0.0f, -1.0f));
		glm::vec3 upward = glm::normalize(rotation * glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), forward));

		glm::vec3 offsetToTarget = m_waypoints[m_currentWPTarget].first - (*m_transformComp).translation;
		float angleToTarget = acos(glm::dot(forward, glm::normalize(offsetToTarget)));

		if (angleToTarget > 0.5f || angleToTarget < -0.5f)
		{
			rotation -= upward * (angleToTarget * m_speed);
			(*m_transformComp).updateTransform();
			return false;
		}
		else { true; }
	};

	bool movingTowardsTarget()
	{

	};

	void generateRandomWaypoints(glm::vec3 wpLowerBounds, glm::vec3 wpUpperBounds, int numWaypoints)
	{
		m_waypoints.reserve(numWaypoints);

		for (int i = 0; i < numWaypoints; i++)
		{

		}
	};
};