#include "SiegeTankAgent.h"
#include "../../Commander/Commander.h"

bool SiegeTankAgent::useAbilities()
{
	//Siege Mode
	if (Broodwar->self()->hasResearched(TechTypes::Tank_Siege_Mode))
	{
		int range = UnitTypes::Terran_Siege_Tank_Siege_Mode.groundWeapon().maxRange();
		bool goSiege = false;

		//Check if enemy ground units are visible
		for (auto &u : Broodwar->enemy()->getUnits())
		{
			if (u->exists() && !u->getType().isFlyer())
			{
				if (unit->getDistance(u) <= range)
				{
					goSiege = true;
				}
			}
		}

		//If we are defending and are at the defense position, go
		//in siege mode
		Squad* sq = Commander::getInstance()->getSquad(squadID);
		if (sq != NULL)
		{
			if (!sq->isActive())
			{
				double d = unit->getDistance(Position(goal));
				if (d <= range * 0.5) goSiege = true;
			}
		}

		if (goSiege && !unit->isSieged())
		{
			unit->siege();
			return true;
		}
		if (!goSiege && unit->isSieged())
		{
			unit->unsiege();
			return true;
		}
	}

	return false;
}
