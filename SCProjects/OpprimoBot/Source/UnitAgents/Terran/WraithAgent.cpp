#include "WraithAgent.h"

bool WraithAgent::useAbilities()
{
	//Cloaking
	TechType cloak = TechTypes::Cloaking_Field;
	if (Broodwar->self()->hasResearched(cloak) && !unit->isCloaked() && unit->getEnergy() >= 25 && !isDetectorWithinRange(unit->getTilePosition(), 192))
	{
		int range = 10 * 32;

		//Check if enemy units are visible
		for (auto &u : Broodwar->enemy()->getUnits())
		{
			if (u->exists())
			{
				if (unit->getDistance(u) <= range)
				{
					unit->useTech(cloak);
					return true;
				}
			}
		}
	}

	return false;
}
