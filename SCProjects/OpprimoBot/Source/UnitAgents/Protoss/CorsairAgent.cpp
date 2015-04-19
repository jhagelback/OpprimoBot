#include "CorsairAgent.h"
#include "../../Pathfinding/NavigationAgent.h"
#include "../../MainAgents/TargetingAgent.h"

bool CorsairAgent::useAbilities()
{
	//Disruption Web
	TechType web = TechTypes::Disruption_Web;
	if (Broodwar->self()->hasResearched(web) && unit->getEnergy() >= 125 && Broodwar->getFrameCount() - lastUseFrame >= 40)
	{
		Unit target = getClosestEnemyAirDefense(320);
		if (target != NULL)
		{
			if (target->getEnsnareTimer() == 0)
			{
				if (unit->useTech(web, target))
				{
					lastUseFrame = Broodwar->getFrameCount();
					Broodwar << "Use Disruption Web on " << target->getType().getName() << endl;
					return true;
				}
			}
		}
	}

	return false;
}

Unit CorsairAgent::getClosestEnemyAirDefense(int maxRange)
{
	Unit enemy = NULL;
	double bestDist = 100000;

	for (auto &u : Broodwar->enemy()->getUnits())
	{
		if (u->exists())
		{
			UnitType type = u->getType();

			bool canAttackAir = false;
			if (type.isBuilding())
			{
				if (type.groundWeapon().targetsAir()) canAttackAir = true;
				if (type.airWeapon().targetsAir()) canAttackAir = true;
			}

			if (canAttackAir)
			{
				double cDist = unit->getDistance(u);
				if (cDist < bestDist)
				{
					bestDist = cDist;
					enemy = u;
				}
			}
		}
	}

	if (bestDist >= 0 && bestDist <= maxRange)
	{
		return enemy;
	}
	else
	{
		return NULL;
	}
}
