#include "VultureAgent.h"

bool VultureAgent::useAbilities()
{
	//Mines
	if (unit->getSpiderMineCount() > 0 && Broodwar->getFrameCount() - mineDropFrame >= 100)
	{
		//Check if enemy units are visible
		for (auto &u : Broodwar->enemy()->getUnits())
		{
			if (u->exists())
			{
				if (unit->getDistance(u) <= unit->getType().sightRange())
				{
					if (unit->useTech(TechTypes::Spider_Mines, unit->getPosition()))
					{
						mineDropFrame = Broodwar->getFrameCount();
						Broodwar << "Spider Mine dropped" << endl;
						return true;
					}
				}
			}
		}
	}

	return false;
}
