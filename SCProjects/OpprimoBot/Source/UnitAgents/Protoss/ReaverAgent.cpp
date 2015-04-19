#include "ReaverAgent.h"

bool ReaverAgent::useAbilities()
{
	int maxLoad = 5;
	if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Reaver_Capacity) > 0)
	{
		maxLoad = 10;
	}

	if(unit->getScarabCount() < maxLoad)
	{
		if (unit->train(UnitTypes::Protoss_Scarab))
		{
			return true;
		}
	}

	return false;
}
