#include "FirebatAgent.h"
#include "../../Managers/AgentManager.h"
#include "../../Commander/Commander.h"

bool FirebatAgent::useAbilities()
{
	//Load into a Bunker
	if (!unit->isLoaded())
	{
		Squad* sq = Commander::getInstance()->getSquad(squadID);
		if (sq != NULL)
		{
			if (sq->isBunkerDefend())
			{
				Agentset agents = AgentManager::getInstance()->getAgents();
				for (auto &a : agents)
				{
					if (a->isAlive() && a->isOfType(UnitTypes::Terran_Bunker) && a->getUnit()->exists())
					{
						if (a->getUnit()->getLoadedUnits().size() < 4)
						{
							unit->rightClick(a->getUnit());
							sq->setBunkerID(a->getUnitID());
							return true;
						}
					}
				}
			}
		}
	}

	//Use Stim Packs
	if (Broodwar->self()->hasResearched(TechTypes::Stim_Packs) && !unit->isStimmed() && unit->getHitPoints() >= 20 && !unit->isLoaded())
	{
		//Check if enemy units are visible
		for (auto &u : Broodwar->enemy()->getUnits())
		{
			if (u->exists())
			{
				if (unit->getDistance(u) <= unit->getType().sightRange())
				{
					unit->useTech(TechTypes::Stim_Packs);
					return true;
				}
			}
		}
	}

	return false;
}
