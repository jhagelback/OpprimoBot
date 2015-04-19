#include "MutaliskAgent.h"
#include "../../Managers/AgentManager.h"
#include "../../Commander/Commander.h"

bool MutaliskAgent::useAbilities()
{
	//Check for morph
	if (AgentManager::getInstance()->countNoUnits(UnitTypes::Zerg_Greater_Spire) > 0)
	{
		Squad* sq = Commander::getInstance()->getSquad(squadID);
		if (sq != NULL)
		{
			if (sq->morphsTo().getID() == UnitTypes::Zerg_Devourer.getID())
			{
				if (enemyUnitsVisible())
				{
					if (Broodwar->canMake(UnitTypes::Zerg_Devourer, unit))
					{
						if (unit->morph(UnitTypes::Zerg_Devourer))
						{
							return true;
						}
					}
				}
			}

			if (sq->morphsTo().getID() == UnitTypes::Zerg_Guardian.getID())
			{
				if (enemyUnitsVisible())
				{
					if (Broodwar->canMake(UnitTypes::Zerg_Guardian, unit))
					{
						if (unit->morph(UnitTypes::Zerg_Guardian))
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
