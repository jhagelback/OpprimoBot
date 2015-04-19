#include "QueenAgent.h"

bool QueenAgent::useAbilities()
{
	//Spawn Broodlings
	if (unit->getEnergy() >= 150)
	{
		//Count enemy units and find an enemy organic unit
		int cntEnemy = 0;
		Unit enemyOrganic = NULL;
		for (auto &u : Broodwar->enemy()->getUnits())
		{
			if (u->exists())
			{
				if (unit->getDistance(u) <= 6 * 32 && u->getIrradiateTimer() == 0)
				{
					cntEnemy++;
					if (u->getType().isOrganic())
					{
						enemyOrganic = u;
					}
				}
			}
		}

		if (cntEnemy >= 5 && enemyOrganic != NULL)
		{
			if (unit->useTech(TechTypes::Spawn_Broodlings, enemyOrganic))
			{
				Broodwar << "Used Spawn Broodlings on " << enemyOrganic->getType().getName() << endl;
				return true;
			}
		}
	}

	return false;
}
