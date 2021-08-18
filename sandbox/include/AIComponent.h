#pragma once
#include "IMAT3905.h"

//! A enumerated list of defined AI behaviours the AIComponent can be in. 
const enum AIBehaviour { Loop = 0, Wander = 1, Pause = 2 };

//! Stores a list of waypoints (positions) the entity moves between in a different order depending on its current AIBehaviour.
class AIComponent
{
public:
	//! Default constructor.
	AIComponent() { };

	//! Constructor to initialise the AIComponent's starting values. Creates a list (the size of numDesiredWpts) of random positions within the specified bounds.
	AIComponent(AIBehaviour startBehaviour, float mSpeed, float tSpeed, glm::ivec2 wpMinBounds, glm::ivec2 wpMaxBounds, int numDesiredWpts) : m_currentBehaviour(startBehaviour), m_moveSpeed(mSpeed), m_turnSpeed(tSpeed)
	{
		m_waypoints.reserve(numDesiredWpts);
		for (int i = 0; i < numDesiredWpts; i++)
		{
			m_waypoints.push_back(std::pair<glm::ivec2, bool>(glm::ivec2(generateRandomInt(wpMinBounds.x, wpMaxBounds.x), generateRandomInt(wpMinBounds.y, wpMaxBounds.y)), false));
		}
	};

	int getCurrentBehavNum() { return m_currentBehaviour; };			//!< Returns the index number of the current AIBehaviour the component is set to.
	int getNumWaypoints() { return m_waypoints.size(); };				//!< Returns the number of waypoints in the component's list.
	glm::ivec2 getWaypoint(int id) { return m_waypoints[id].first; };	//!< Returns the position of the waypoint at the specified index number in this component's list.
	float getAngle2Target() { return m_angleToTarget; }					//!< Returns the component's angle from its current target waypoint's position.
	glm::vec3 getDist2Target() { return m_offsetToTarget; }				//!< Returns the component's offset from its current target waypoint's position.
	
	//! Returns whether or not the specified index number is the waypoint of the component's current target point in its vector list.
	bool waypointIsTarget(int id)
	{
		if (id == m_cWPTarget) { return true; }
		else { return false; }
	};

	//! Adds a random new waypoint position to the component's vector list.
	void addWaypoint()
	{
		m_waypoints.reserve(m_waypoints.size() + 1);
		m_waypoints.push_back(std::pair<glm::ivec2, bool>({generateRandomInt(-5,5), generateRandomInt(-5,5)}, false));
	};
	//! Adds a specified new integer vec2 waypoint position to the component's vector list.
	void addWaypoint(glm::ivec2 newWP)
	{
		m_waypoints.reserve(m_waypoints.size() + 1);
		m_waypoints.push_back(std::pair<glm::ivec2, bool>(newWP, false));
	};
	//! Adds a specified waypoint position to the component's vector list via 2 int X and Z positions.
	void addWaypoint(int wpX, int wpZ)
	{
		m_waypoints.reserve(m_waypoints.size() + 1);
		m_waypoints.push_back(std::pair<glm::ivec2, bool>(glm::ivec2(wpX, wpZ), false));
	};
	//! Adds a list of specified waypoint positions to the component's vector list.
	void addWaypoint(std::vector<glm::ivec2> wayPts)
	{
		m_waypoints.reserve(m_waypoints.size() + wayPts.size());
		for (auto& point : wayPts)
		{
			m_waypoints.push_back(std::pair<glm::ivec2, bool>(point, false));
		}
	};

	//! Sets the component's current AIBehaviour.
	void setBehaviour(AIBehaviour newBehav)
	{
		if (m_currentBehaviour != newBehav)
		{
			m_currentBehaviour = newBehav;
		}
	};

	//! Allows for the modification of the waypoint at the specified position in the vector list via 2 integer values. 
	void editWaypoint(int pointNum, int newPosX, int newPosZ) { editWaypoint(pointNum, glm::ivec2(newPosX, newPosZ)); };
	//! Allows for the modification of the waypoint at the specified position in the vector list via the revised integer vec2. 
	void editWaypoint(int pointNum, glm::ivec2 newPos)
	{
		if ((pointNum >= 0) && (pointNum < m_waypoints.size()))
		{
			m_waypoints[pointNum].first = newPos;
			m_waypoints[pointNum].second = false;
		}
	};

	//! Deletes the waypoint at the beginning of the vector list.
	void deleteWaypoint()
	{
		m_waypoints.erase(m_waypoints.begin());
		if (m_cWPTarget == m_waypoints.size()) { setNextTarget(AIBehaviour::Loop); } // Swaps the current waypoint target if it would no longer be in the vector list now.
	};

	//! Updates the AIComponent according to its current AIBehaviour. Passes in the component's TransformComponent and current update time.
	void update(float timestep, TransformComponent* transformComp)
	{
		// Don't do anything if paused, otherwise find the component's current waypoint.
		if (m_currentBehaviour != AIBehaviour::Pause)
		{
			if (m_waypoints.size() > 1)
			{
				findWaypoint(timestep, transformComp);
			}
			else { m_currentBehaviour = AIBehaviour::Pause; }
		}
	};
private:
	float m_foundDist = 0.05f;	//!< Leeway amount to qualify the entity as having reached or rotated towards the current waypoint enough. (Difficult to get the entity to the exact position which will be caught between updates).
	float m_moveSpeed;			//!< Speed to move the entity towards its current target waypoint.
	float m_turnSpeed;			//!< Speed to rotate the entity towards its current target waypoint.
	AIBehaviour m_currentBehaviour; //!< Component's current behaviour kind (e.g. Looping through list of waypoints, randomly moving between them, or being paused).
	std::vector<std::pair<glm::ivec2, bool>> m_waypoints; //!< A list of positions for the entity to move between and a boolean to indicate whether they've been visited yet.

	static int m_cWPTarget;		//!< Current index position of the waypoint the entity should currently be heading towards from the m_waypoints vector list.
	glm::vec3 m_cForward;		//!< Current forward directional vector relative to the worldspace directions and the entity's rotation.
	glm::vec3 m_cUpward;		//!< Current upward directional vector relative to the worldspace directions and the entity's rotation.

	static float m_angleToTarget;		//!< Current angle from the entity's front to the component's target waypoint position.
	static	glm::vec3 m_offsetToTarget; //!< Difference in position of the entity's TranformComponent's translation to the component's target waypoint position.

	//! Generates a random integer between the range of the specified minimum and maximum values.
	int generateRandomInt(int min, int max)
	{
		return min + rand() % (max - min + 1);
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
		glm::vec3 waypntPos = glm::vec3(m_waypoints[m_cWPTarget].first.x, transformComp->translation.y, m_waypoints[m_cWPTarget].first.y);

		m_cForward = glm::normalize(transformComp->rotation * glm::vec3(0.0f, 0.0f, -1.0f));
		m_cUpward = glm::normalize(transformComp->rotation * glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), m_cForward));

		m_offsetToTarget = waypntPos - transformComp->translation;
		m_angleToTarget = acos(glm::dot(m_cForward, glm::normalize(m_offsetToTarget)));

		m_offsetToTarget = roundTo2Decimal(m_offsetToTarget);
		m_angleToTarget = roundTo2Decimal(m_angleToTarget);
	};

	bool facingTarget(TransformComponent* transformComp)
	{
		calcCurrentVectors(transformComp);

		if ((m_angleToTarget < m_foundDist) && (m_angleToTarget > -m_foundDist))
		{
			return true;
		}
		else { return false; }
	};

	bool reachedTarget(TransformComponent* transformComp)
	{
		calcCurrentVectors(transformComp);

		if ((m_offsetToTarget.x < m_foundDist) && (m_offsetToTarget.x > -m_foundDist) &&
			(m_offsetToTarget.z < m_foundDist) && (m_offsetToTarget.z > -m_foundDist))
		{
			m_waypoints[m_cWPTarget].second = true;
			if (m_waypoints.size() == 1) { m_currentBehaviour == AIBehaviour::Pause; }
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

	void findWaypoint(float timestep, TransformComponent* transformComp)
	{
		if (!m_waypoints[m_cWPTarget].second)
		{
			if (!reachedTarget(transformComp))
			{
				if (facingTarget(transformComp))
				{
					moveToTarget(timestep, transformComp);
				}
				else { turnToTarget(timestep, transformComp); }
			}
			else { setNextTarget(m_currentBehaviour); }
		}
	}

	void setNextTarget(AIBehaviour behaviour) 
	{
		if (m_waypoints.size() < 1)
		{
			m_cWPTarget = 0;
			m_currentBehaviour = AIBehaviour::Pause;
			return;
		}

		switch (behaviour)
		{
		case AIBehaviour::Loop:
			if (m_cWPTarget < (m_waypoints.size() - 1))
			{
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
			m_cWPTarget = generateRandomInt(0, m_waypoints.size());
			m_waypoints[m_cWPTarget].second = false;
			break;
		}
	}
};