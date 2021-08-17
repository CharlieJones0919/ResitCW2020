#include "editor.h"

bool AIComponent::waypointIsTarget(int id)
{
	if (id == m_cWPTarget) { return true; }
	else { return false; }
}
