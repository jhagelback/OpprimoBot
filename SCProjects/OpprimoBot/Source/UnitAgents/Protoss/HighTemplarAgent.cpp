#include "HighTemplarAgent.h"
#include "../../Pathfinding/NavigationAgent.h"
#include "../../Managers/AgentManager.h"
#include "../../Commander/Commander.h"

bool HighTemplarAgent::useAbilities()
{
	if (isOfType(unit->getType(), UnitTypes::Protoss_High_Templar))
	{
		//Not transformed to Archon. Use spells.

		//PSI storm
		TechType psiStorm = TechTypes::Psionic_Storm;
		if (Broodwar->self()->hasResearched(psiStorm) && unit->getEnergy() >= 75)
		{
			int range = TechTypes::Psionic_Storm.getWeapon().maxRange();
			//Check if enemy units are visible
			for (auto &u : Broodwar->enemy()->getUnits())
			{
				if (u->exists())
				{
					if (unit->getDistance(u) <= range && !u->isUnderStorm())
					{
						if (unit->useTech(psiStorm, u->getPosition()))
						{
							Broodwar << "Psionic Storm used on " << u->getType().getName() << endl;
							return true;
						}
					}
				}
			}
		}

		//Hallucination
		TechType hallucination = TechTypes::Hallucination;
		if (Broodwar->self()->hasResearched(hallucination) && unit->getEnergy() >= 100)
		{
			//Check if enemy units are visible
			for (auto &u : Broodwar->enemy()->getUnits())
			{
				if (u->exists())
				{
					if (unit->getDistance(u) <= unit->getType().sightRange())
					{
						BaseAgent* target = findHallucinationTarget();
						if (target != NULL)
						{
							if (unit->useTech(hallucination, target->getUnit()))
							{
								Broodwar << "Uses Hallucination on " << target->getUnitType().getName() << endl;
								return true;
							}
						}
					}
				}
			}
		}

		//Morph to Archon	
		if (!unit->isBeingConstructed())
		{
			Squad* sq = Commander::getInstance()->getSquad(squadID);
			if (sq != NULL)
			{
				if (sq->morphsTo().getID() == UnitTypes::Protoss_Archon.getID() || unit->getEnergy() < 50)
				{
					if (!enemyUnitsVisible() && !hasCastTransform)
					{
						BaseAgent* target = findArchonTarget();
						if (target != NULL)
						{
							if (unit->useTech(TechTypes::Archon_Warp, target->getUnit()))
							{
								hasCastTransform = true;
								return true;
							}
						}
					}
				}
			}
		}
	}
	
	return false;
}


BaseAgent* HighTemplarAgent::findHallucinationTarget()
{
	int maxRange = TechTypes::Hallucination.getWeapon().maxRange();

	Agentset agents = AgentManager::getInstance()->getAgents();
	for (auto &a : agents)
	{
		bool targetUnit = false;
		if (a->isOfType(UnitTypes::Protoss_Carrier)) targetUnit = true;
		if (a->isOfType(UnitTypes::Protoss_Scout)) targetUnit = true;
		if (a->isOfType(UnitTypes::Protoss_Archon)) targetUnit = true;
		if (a->isOfType(UnitTypes::Protoss_Reaver)) targetUnit = true;

		if (a->isAlive() && targetUnit)
		{
			if (!a->getUnit()->isHallucination())
			{
				return a;
			}
		}
	}
	
	return NULL;
}

BaseAgent* HighTemplarAgent::findArchonTarget()
{
	Squad* mSquad = Commander::getInstance()->getSquad(squadID);
	if (mSquad != NULL)
	{
		Agentset agents = mSquad->getMembers();
		for (auto &a : agents)
		{
			if (a->isAlive() && a->getUnitID() != unitID && a->isOfType(UnitTypes::Protoss_High_Templar) && !a->getUnit()->isBeingConstructed())
			{
				double dist = a->getUnit()->getPosition().getDistance(unit->getPosition());
				if (dist <= 64)
				{
					return a;
				}
			}
		}
	}

	return NULL;
}

int HighTemplarAgent::friendlyUnitsWithinRange(TilePosition tilePos, int maxRange)
{
	int fCnt = 0;
	Agentset agents = AgentManager::getInstance()->getAgents();
	for (auto &a : agents)
	{
		if (a->isUnit() && !a->isOfType(UnitTypes::Terran_Medic))
		{
			double dist = a->getUnit()->getDistance(Position(tilePos));
			if (dist <= maxRange)
			{
				fCnt++;
			}
		}
	}
	return fCnt;
}
