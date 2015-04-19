#include "LurkerAgent.h"
#include "../../Pathfinding/NavigationAgent.h"
#include "../../MainAgents/TargetingAgent.h"

bool LurkerAgent::useAbilities()
{
	//Check if enemy units are visible
	bool enemyVisible = false;
	for (auto &u : Broodwar->enemy()->getUnits())
	{
		if (u->exists())
		{
			if (unit->getDistance(u) <= unit->getType().sightRange() && !u->getType().isFlyer())
			{
				enemyVisible = true;
				break;
			}
		}
	}

	if (enemyVisible && !unit->isBurrowed())
	{
		if (unit->burrow())
		{
			return true;
		}
	}
	if (!enemyVisible && unit->isBurrowed())
	{
		if (unit->unburrow())
		{
			return true;
		}
	}

	return false;
}
