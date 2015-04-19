#include "HydraliskAgent.h"
#include "../../Commander/Commander.h"

bool HydraliskAgent::useAbilities()
{
	if (Broodwar->self()->hasResearched(TechTypes::Lurker_Aspect))
	{
		Squad* sq = Commander::getInstance()->getSquad(squadID);
		if (sq != NULL)
		{
			if (sq->morphsTo().getID() == UnitTypes::Zerg_Lurker.getID())
			{
				if (!enemyUnitsVisible())
				{
					if (Broodwar->canMake(UnitTypes::Zerg_Lurker, unit))
					{
						if (unit->morph(UnitTypes::Zerg_Lurker))
						{
							return true;
						}
					}
				}
			}
		}
	}
	
	return false;
}
