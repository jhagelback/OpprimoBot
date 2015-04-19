#include "MedicAgent.h"
#include "../../Managers/AgentManager.h"

bool MedicAgent::useAbilities()
{
	//Check heal
	double bestDist = -1;
	Unit toHeal = NULL;

	Agentset agents = AgentManager::getInstance()->getAgents();
	for (auto &a : agents)
	{
		if (a->isAlive() && a->isDamaged())
		{
			if (isMedicTarget(a->getUnit()) && a->getUnitID() != unit->getID())
			{
				Unit cUnit = a->getUnit();
				if (cUnit->exists() && cUnit->getHitPoints() > 0)
				{
					double dist = unit->getDistance(cUnit);
					if (bestDist < 0 || dist < bestDist)
					{
						bestDist = dist;
						toHeal = cUnit;
					}
				}
			}
		}
	}

	if (bestDist >= 0 && toHeal != NULL)
	{
		unit->useTech(TechTypes::Healing, toHeal);
		return true;
	}

	return false;
}

bool MedicAgent::isMedicTarget(Unit mUnit)
{
	if (!mUnit->getType().isOrganic())
	{
		//Can only heal organic units
		return false;
	}

	if (mUnit->getType().isWorker())
	{
		//We can heal workers, but no point
		//in following them
		return false;
	}

	if (!mUnit->getType().canAttack())
	{
		//Dont follow units that cant attack
		return false;
	}

	if (mUnit->getType().getID() == UnitTypes::Terran_Medic.getID())
	{
		//Dont follow other medics
		return false;
	}

	if (mUnit->isLoaded())
	{
		//Dont "follow" bunkered units
		return false;
	}

	return true;
}
